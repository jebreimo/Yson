//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Reader.hpp"

#include "../Ystring/Conversion.hpp"
#include "../Ystring/Escape/Escape.hpp"
#include "Detail/Base64.hpp"
#include "Detail/GetValueType.hpp"
#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "Detail/ParseDouble.hpp"
#include "Detail/ParseInteger.hpp"
#include "JsonSingleValue.hpp"
#include "Detail/TextBufferReader.hpp"
#include "Detail/TextFileReader.hpp"

namespace Yson
{
    using std::get;

    const size_t DEFAULT_CHUNK_SIZE = 16 * 1024;

    #define READER_THROW(msg) \
        throw ReaderException((msg), __FILE__, __LINE__, __FUNCTION__, \
                                  lineNumber(), columnNumber())

    namespace
    {
        std::pair<unsigned, unsigned> countRowsAndColumns(
                std::pair<const char*, const char*> token)
        {
            unsigned rows = 0, cols = 0;
            for (auto it = token.first; it != token.second; ++it)
            {
                if (*it == '\n')
                {
                    ++rows;
                    cols = 0;
                }
                else if (*it == '\r')
                {
                    cols = 0;
                }
                else
                {
                    ++cols;
                }
            }
            return std::make_pair(rows, cols);
        }

        bool equalsNull(const char* first, const char* last)
        {
            return last - first == 4
                   && std::equal(first, last, "null");
        }

        bool equalsTrue(const char* first, const char* last)
        {
            return last - first == 4
                   && std::equal(first, last, "true");
        }

        bool equalsFalse(const char* first, const char* last)
        {
            return last - first == 5
                   && std::equal(first, last, "false");
        }

        bool isStartOfStructure(TokenType tokenType)
        {
            return tokenType == TokenType::START_ARRAY
                   || tokenType == TokenType::START_OBJECT;
        }
    }

    Reader::Reader(std::istream& stream)
        : m_TextReader(new TextStreamReader(stream)),
          m_ChunkSize(DEFAULT_CHUNK_SIZE),
          m_State(INITIAL_STATE),
          m_LanguageExtensions(0),
          m_SkipElementDepth(0)
    {}

    Reader::Reader(const std::string& fileName)
        : m_TextReader(new TextFileReader(fileName)),
          m_ChunkSize(DEFAULT_CHUNK_SIZE),
          m_State(INITIAL_STATE),
          m_LanguageExtensions(0),
          m_SkipElementDepth(0)
    {}

    Reader::Reader(const char* buffer, size_t bufferSize)
            : m_TextReader(new TextBufferReader(buffer, bufferSize)),
              m_ChunkSize(DEFAULT_CHUNK_SIZE),
              m_State(INITIAL_STATE),
              m_LanguageExtensions(0),
              m_SkipElementDepth(0)
    {}

    Reader::~Reader()
    {}

    bool Reader::nextKey()
    {
        if (m_SkipElementDepth != 0 && m_State != UNRECOVERABLE_ERROR)
            skipElement();

        switch (m_State)
        {
        case AT_START_OF_OBJECT:
        case AT_KEY_IN_OBJECT:
        case AFTER_KEY_IN_OBJECT:
        case AT_COLON_IN_OBJECT:
        case AFTER_VALUE_IN_OBJECT:
        case AT_COMMA_IN_OBJECT:
            break;
        case AT_VALUE_IN_OBJECT:
            skipElement();
            break;
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
            return false;
        default:
            READER_THROW("nextKey() can only be called inside an object.");
        }
        while (nextTokenImpl())
        {
            switch (m_State)
            {
            case AT_VALUE_IN_OBJECT:
                skipElement();
                break;
            case AT_KEY_IN_OBJECT:
                return true;
            case AT_START_OF_OBJECT:
            case AFTER_KEY_IN_OBJECT:
            case AT_COLON_IN_OBJECT:
            case AFTER_VALUE_IN_OBJECT:
            case AT_COMMA_IN_OBJECT:
                break;
            default:
                return false;
            }
        }
        return false;
    }

    bool Reader::nextValue()
    {
        if (m_SkipElementDepth != 0 && m_State != UNRECOVERABLE_ERROR)
            skipElement();

        switch (m_State)
        {
        case AT_END_OF_STREAM:
        case AT_END_OF_DOCUMENT:
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
            return false;
        case AT_VALUE_IN_OBJECT:
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_OF_DOCUMENT:
            if (isStartOfStructure(m_Tokenizer.tokenType()))
                skipElement();
            break;
        default:
            break;
        }
        while (nextTokenImpl())
        {
            switch (m_State)
            {
            case AT_VALUE_IN_OBJECT:
            case AT_VALUE_IN_ARRAY:
            case AT_VALUE_OF_DOCUMENT:
                return true;
            default:
                break;
            }
        }
        return false;
    }

    bool Reader::nextToken()
    {
        if (m_SkipElementDepth != 0 && m_State != UNRECOVERABLE_ERROR)
            skipElement();

        switch (m_State)
        {
        case AT_VALUE_OF_DOCUMENT:
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_IN_OBJECT:
            if (isStartOfStructure(m_Tokenizer.tokenType()))
                skipElement();
            break;
        default:
            break;
        }

        return nextTokenImpl();
    }

    void Reader::enter()
    {
        State pushState;
        switch (m_State)
        {
        case AT_VALUE_IN_ARRAY:
            pushState = AFTER_VALUE_IN_ARRAY;
            break;
        case AT_VALUE_OF_DOCUMENT:
            pushState = AT_END_OF_DOCUMENT;
            break;
        case AT_VALUE_IN_OBJECT:
            pushState = AFTER_VALUE_IN_OBJECT;
            break;
        default:
            READER_THROW("There's no array or object to enter.");
        }
        if (m_Tokenizer.tokenType() == TokenType::START_OBJECT)
            m_State = AT_START_OF_OBJECT;
        else if (m_Tokenizer.tokenType() == TokenType::START_ARRAY)
            m_State = AT_START_OF_ARRAY;
        else if (isEnterNullEnabled() && isNull())
            m_State = AT_END_OF_NULL;
        else
            READER_THROW("Only arrays and objects can be entered.");
        m_StateStack.push(pushState);
    }

    void Reader::leave()
    {
        switch (m_State)
        {
        case INITIAL_STATE:
        case AT_START_OF_DOCUMENT:
        case AT_VALUE_OF_DOCUMENT:
        case AT_END_OF_DOCUMENT:
        case AT_END_OF_STREAM:
            READER_THROW("leave() wasn't preceded by enter().");
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        default:
            while (nextToken());
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        }
    }

    ValueType Reader::valueType() const
    {
        switch (m_Tokenizer.tokenType())
        {
        case TokenType::START_ARRAY:
            return ValueType::ARRAY;
        case TokenType::START_OBJECT:
            return ValueType::OBJECT;
        case TokenType::STRING:
            return ValueType::STRING;
        case TokenType::VALUE:
            break;
        default:
            READER_THROW("Current token has no value type.");
        }

        auto valueType = getValueType(m_Tokenizer.rawToken());
        switch (valueType)
        {
        case ValueType::BIN_INTEGER:
        case ValueType::OCT_INTEGER:
        case ValueType::HEX_INTEGER:
            if (isExtendedIntegersEnabled())
                return ValueType::INTEGER;
            READER_THROW("Invalid value.");
        case ValueType::EXTENDED_FLOAT:
            if (isExtendedFloatsEnabled())
                return ValueType::FLOAT;
            READER_THROW("Invalid value.");
        case ValueType::INVALID:
            READER_THROW("Invalid value.");
        default:
            return valueType;
        }
    }

    ValueType Reader::stringValueType() const
    {
        auto type = valueType();
        if (type != ValueType::STRING)
            return type;

        auto token = m_Tokenizer.rawToken();
        ++token.first;
        --token.second;

        auto valueType = getValueType(token);
        switch (valueType)
        {
        case ValueType::INTEGER:
        case ValueType::FLOAT:
        case ValueType::TRUE_VALUE:
        case ValueType::FALSE_VALUE:
        case ValueType::NULL_VALUE:
            return valueType;
        case ValueType::BIN_INTEGER:
        case ValueType::OCT_INTEGER:
        case ValueType::HEX_INTEGER:
            return ValueType::INTEGER;
        case ValueType::EXTENDED_FLOAT:
            return ValueType::FLOAT;
        default:
            return ValueType::STRING;
        }
    }

    TokenType Reader::tokenType() const
    {
        return m_Tokenizer.tokenType();
    }

    std::string Reader::token() const
    {
        return m_Tokenizer.token();
    }

    std::pair<const char*, const char*> Reader::rawToken() const
    {
        return m_Tokenizer.rawToken();
    }

    size_t Reader::lineNumber() const
    {
        return m_LineNumberCounter.line();
    }

    size_t Reader::columnNumber() const
    {
        return m_LineNumberCounter.column();
    }

    bool Reader::isNull() const
    {
        auto token = getValueToken();
        return equalsNull(get<0>(token), get<1>(token));
    }

    void Reader::readValue(bool& value) const
    {
        auto token = getValueToken();
        if (equalsTrue(get<0>(token), get<1>(token)))
            value = true;
        else if (equalsFalse(get<0>(token), get<1>(token)))
            value = false;
        else
            READER_THROW("Invalid boolean value");
    }

    void Reader::readValue(int8_t& value) const
    {
        readSignedInteger(value);
    }

    void Reader::readValue(int16_t& value) const
    {
        readSignedInteger(value);
    }

    void Reader::readValue(int32_t& value) const
    {
        readSignedInteger(value);
    }

    void Reader::readValue(int64_t& value) const
    {
        readSignedInteger(value);
    }

    void Reader::readValue(uint8_t& value) const
    {
        readUnsignedInteger(value);
    }

    void Reader::readValue(uint16_t& value) const
    {
        readUnsignedInteger(value);
    }

    void Reader::readValue(uint32_t& value) const
    {
        readUnsignedInteger(value);
    }

    void Reader::readValue(uint64_t& value) const
    {
        readUnsignedInteger(value);
    }

    void Reader::readValue(float& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseFloat(get<0>(token), get<1>(token));
        if (!parsedValue.second)
            READER_THROW("Invalid floating point value");
        value = parsedValue.first;
    }

    void Reader::readValue(double& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseDouble(get<0>(token), get<1>(token));
        if (!parsedValue.second)
            READER_THROW("Invalid floating point value");
        value = parsedValue.first;
    }

    void Reader::readValue(long double& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseLongDouble(get<0>(token), get<1>(token));
        if (!parsedValue.second)
            READER_THROW("Invalid floating point value");
        value = parsedValue.first;
    }

    void Reader::readValue(std::string& value) const
    {
        auto token = m_Tokenizer.rawToken();
        if (m_Tokenizer.tokenType() == TokenType::STRING)
        {
            ++token.first;
            --token.second;
        }
        else if (m_Tokenizer.tokenType() == TokenType::BLOCK_STRING)
        {
            token.first += 3;
            token.second -= 3;
        }
        else if (!isValuesAsStringsEnabled()
                 || m_Tokenizer.tokenType() != TokenType::VALUE)
        {
            READER_THROW("Current token is not a string.");
        }
        value.assign(token.first, token.second);
        if (Ystring::hasEscapedCharacters(value))
            value = Ystring::unescape(value);
    }

    void Reader::readBase64(std::vector<uint8_t>& value) const
    {
        std::string rawValue;
        readValue(rawValue);
        try
        {
            value = fromBase64(rawValue);
        }
        catch (std::exception& ex)
        {
            READER_THROW(ex.what());
        }
    }


    std::unique_ptr<JsonValue> Reader::read()
    {
        switch (m_State)
        {
        case AT_VALUE_OF_DOCUMENT:
        case AT_VALUE_IN_OBJECT:
        case AT_VALUE_IN_ARRAY:
            break;
        case AT_END_OF_DOCUMENT:
            if (!nextDocument() || !nextValue())
                return std::unique_ptr<JsonValue>();
            break;
        case INITIAL_STATE:
        case AT_START_OF_DOCUMENT:
        case AT_START_OF_OBJECT:
        case AT_START_OF_ARRAY:
        case AT_KEY_IN_OBJECT:
        case AFTER_KEY_IN_OBJECT:
        case AT_COLON_IN_OBJECT:
        case AFTER_VALUE_IN_OBJECT:
        case AT_COMMA_IN_OBJECT:
        case AFTER_VALUE_IN_ARRAY:
        case AT_COMMA_IN_ARRAY:
            if (!nextValue())
                return std::unique_ptr<JsonValue>();
            break;
        case AT_END_OF_STREAM:
        case UNRECOVERABLE_ERROR:
        case AT_END_OF_ARRAY:
        case AT_END_OF_NULL:
        case AT_END_OF_OBJECT:
            return std::unique_ptr<JsonValue>();
        }

        auto buffer = std::make_shared<std::string>();
        auto result = readCompleteValue(buffer);
        result->updateStringRef(0);
        return result;
    }

    std::unique_ptr<JsonValue>
    Reader::readCompleteArray(const std::shared_ptr<std::string>& buffer)
    {
        std::vector<std::unique_ptr<JsonValue>> tokens;
        enter();
        while (nextValue())
            tokens.push_back(readCompleteValue(buffer));
        leave();
        return std::unique_ptr<JsonValue>(new JsonArray(move(tokens)));
    }

    std::unique_ptr<JsonValue>
    Reader::readCompleteObject(const std::shared_ptr<std::string>& buffer)
    {
        std::vector<std::pair<std::string, std::unique_ptr<JsonValue>>> tokens;
        enter();
        while (nextKey())
        {
            auto key = Yson::read<std::string>(*this);
            nextValue();
            tokens.push_back({key, readCompleteValue(buffer)});
        }
        leave();
        return std::unique_ptr<JsonValue>(new JsonObject(move(tokens)));
    }

    std::unique_ptr<JsonValue>
    Reader::readCompleteValue(const std::shared_ptr<std::string>& buffer)
    {
        std::unique_ptr<JsonValue> token;
        auto tokType = tokenType();
        switch (tokType)
        {
        case TokenType::START_ARRAY:
            token = readCompleteArray(buffer);
            break;
        case TokenType::START_OBJECT:
            token = readCompleteObject(buffer);
            break;
        case TokenType::VALUE:
        case TokenType::STRING:
        case TokenType::BLOCK_STRING:
        {
            auto rawToken = getValueToken();
            buffer->append(std::get<0>(rawToken), std::get<1>(rawToken));
            auto size = std::get<1>(rawToken) - std::get<0>(rawToken);
            token = std::unique_ptr<JsonValue>(new JsonSingleValue(
                    StringRef(nullptr, size), buffer, tokType));
            break;
        }
        default:
            break;
        }
        return token;
    }

    bool Reader::nextDocument()
    {
        while (m_State != AT_END_OF_DOCUMENT
               && m_State != AT_START_OF_DOCUMENT)
        {
            switch (m_State)
            {
            case INITIAL_STATE:
                fillBuffer();
                m_State = AT_START_OF_DOCUMENT;
                break;
            case AT_END_OF_STREAM:
                return false;
            case AT_END_OF_OBJECT:
            case AT_END_OF_ARRAY:
            case AT_END_OF_NULL:
                leave();
                break;
            default:
                nextValue();
                break;
            }
        }
        m_State = AT_START_OF_DOCUMENT;
        skipWhitespace();
        return m_Tokenizer.peek().first != TokenType::END_OF_BUFFER;
    }

    bool Reader::isStringsAsValuesEnabled() const
    {
        return languageExtension(STRINGS_AS_VALUES);
    }

    Reader& Reader::setStringsAsValuesEnabled(bool value)
    {
        return setLanguageExtension(STRINGS_AS_VALUES, value);
    }

    bool Reader::isValuesAsStringsEnabled() const
    {
        return languageExtension(VALUES_AS_STRINGS);
    }

    Reader& Reader::setValuesAsStringsEnabled(bool value)
    {
        return setLanguageExtension(VALUES_AS_STRINGS, value);
    }

    bool Reader::isEndElementAfterCommaEnabled() const
    {
        return languageExtension(END_ELEMENT_AFTER_COMMA);
    }

    Reader& Reader::setEndElementAfterCommaEnabled(bool value)
    {
        return setLanguageExtension(END_ELEMENT_AFTER_COMMA, value);
    }

    bool Reader::isCommentsEnabled() const
    {
        return languageExtension(COMMENTS);
    }

    Reader& Reader::setCommentsEnabled(bool value)
    {
        return setLanguageExtension(COMMENTS, value);
    }

    bool Reader::isEnterNullEnabled() const
    {
        return languageExtension(ENTER_NULL);
    }

    Reader& Reader::setEnterNullEnabled(bool value)
    {
        return setLanguageExtension(ENTER_NULL, value);
    }

    bool Reader::isValuesAsKeysEnabled() const
    {
        return languageExtension(VALUES_AS_KEYS);
    }

    Reader& Reader::setValuesAsKeysEnabled(bool value)
    {
        return setLanguageExtension(VALUES_AS_KEYS, value);
    }

    bool Reader::isExtendedIntegersEnabled() const
    {
        return languageExtension(EXTENDED_INTEGERS);
    }

    Reader& Reader::setExtendedIntegersEnabled(bool value)
    {
        return setLanguageExtension(EXTENDED_INTEGERS, value);
    }

    bool Reader::isBlockStringsEnabled() const
    {
        return languageExtension(BLOCK_STRINGS);
    }

    Reader& Reader::setBlockStringsEnabled(bool value)
    {
        return setLanguageExtension(BLOCK_STRINGS, value);
    }

    bool Reader::isExtendedFloatsEnabled() const
    {
        return languageExtension(EXTENDED_FLOATS);
    }

    Reader& Reader::setExtendedFloatsEnabled(bool value)
    {
        return setLanguageExtension(EXTENDED_FLOATS, value);
    }

    int Reader::languageExtensions() const
    {
        return m_LanguageExtensions;
    }

    Reader& Reader::setLanguageExtensions(int value)
    {
        m_LanguageExtensions = value;
        return *this;
    }

    bool Reader::languageExtension(
            Reader::LanguageExtensions ext) const
    {
        return (m_LanguageExtensions & ext) != 0;
    }

    Reader& Reader::setLanguageExtension(
            Reader::LanguageExtensions ext, bool value)
    {
        if (value)
            m_LanguageExtensions |= ext;
        else
            m_LanguageExtensions &= ~ext;
        return *this;
    }

    bool Reader::fillBuffer()
    {
        auto initialSize = m_Buffer.size();
        m_Buffer.clear();
        if (!m_TextReader->read(m_Buffer, m_ChunkSize) && initialSize == 0)
            return false;
        m_Tokenizer.setBuffer(m_Buffer.data(), m_Buffer.size());
        return true;
    }

    bool Reader::nextTokenImpl()
    {
        switch (m_State)
        {
        case UNRECOVERABLE_ERROR:
            READER_THROW("Can't continue reading the current stream.");
        case INITIAL_STATE:
            fillBuffer();
            m_State = AT_START_OF_DOCUMENT;
            break;
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_IN_OBJECT:
            if (isStartOfStructure(m_Tokenizer.tokenType()))
                return false;
            break;
        case AT_VALUE_OF_DOCUMENT:
            if (isStartOfStructure(m_Tokenizer.tokenType()))
                return false;
            m_State = AT_END_OF_DOCUMENT;
            return false;
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
        case AT_END_OF_STREAM:
        case AT_END_OF_DOCUMENT:
            return false;
        default:
            break;
        }

        m_LineNumberCounter.applyNextLineAndColumn();

        bool filledBuffer;

        do
        {
            filledBuffer = false;

            m_Tokenizer.next();
            auto tok = m_Tokenizer.rawToken();
            m_LineNumberCounter.setNextLineAndColumnOffsets(
                    0, int(tok.second - tok.first));

            switch (m_Tokenizer.tokenType())
            {
            case TokenType::START_ARRAY:
                processStartArray();
                break;
            case TokenType::END_ARRAY:
                processEndArray();
                break;
            case TokenType::START_OBJECT:
                processStartObject();
                break;
            case TokenType::END_OBJECT:
                processEndObject();
                break;
            case TokenType::COLON:
                processColon();
                break;
            case TokenType::COMMA:
                processComma();
                break;
            case TokenType::STRING:
                processString();
                break;
            case TokenType::VALUE:
                processValue();
                break;
            case TokenType::END_OF_BUFFER:
                if (fillBuffer())
                    filledBuffer = true;
                else
                    processEndOfStream();
                break;
            case TokenType::BLOCK_STRING:
            {
                processBlockString();
                auto lineCol = countRowsAndColumns(m_Tokenizer.rawToken());
                m_LineNumberCounter.setNextLineAndColumnOffsets(
                        lineCol.first, lineCol.second);
                break;
            }
            case TokenType::COMMENT:
                if (!isCommentsEnabled())
                    READER_THROW("Invalid token.");
                processWhitespace();
                break;
            case TokenType::BLOCK_COMMENT:
            {
                if (!isCommentsEnabled())
                    READER_THROW("Invalid token.");
                processWhitespace();
                auto lineCol = countRowsAndColumns(m_Tokenizer.rawToken());
                m_LineNumberCounter.setNextLineAndColumnOffsets(
                        lineCol.first, lineCol.second);
                break;
            }
            case TokenType::WHITESPACE:
                processWhitespace();
                break;
            case TokenType::NEWLINE:
                processWhitespace();
                m_LineNumberCounter.setNextLineAndColumnOffsets(1, 0);
                break;
            case TokenType::INVALID_TOKEN:
                READER_THROW("Invalid token.");
            }
        } while (filledBuffer);
        return true;
    }

    void Reader::processBlockString()
    {
        if (!isBlockStringsEnabled())
            READER_THROW("Invalid token.");

        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
            m_State = AT_VALUE_OF_DOCUMENT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_VALUE_IN_ARRAY;
            break;
        case AT_COLON_IN_OBJECT:
            m_State = AT_VALUE_IN_OBJECT;
            break;
        default:
            READER_THROW("Unexpected block string.");
        }
    }

    void Reader::processColon()
    {
        switch (m_State)
        {
        case AT_KEY_IN_OBJECT:
        case AFTER_KEY_IN_OBJECT:
            m_State = AT_COLON_IN_OBJECT;
            break;
        case AT_START_OF_OBJECT:
        case AT_COMMA_IN_OBJECT:
        case AT_VALUE_IN_OBJECT:
        case AFTER_VALUE_IN_OBJECT:
            m_State = AT_COLON_IN_OBJECT;
            READER_THROW("Unexpected colon.");
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected colon.");
        }
    }

    void Reader::processComma()
    {
        switch (m_State)
        {
        case AT_VALUE_IN_ARRAY:
        case AFTER_VALUE_IN_ARRAY:
            m_State = AT_COMMA_IN_ARRAY;
            break;
        case AT_VALUE_IN_OBJECT:
        case AFTER_VALUE_IN_OBJECT:
            m_State = AT_COMMA_IN_OBJECT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_COMMA_IN_ARRAY;
            READER_THROW("Unexpected comma.");
        case AT_START_OF_OBJECT:
        case AT_COMMA_IN_OBJECT:
        case AT_KEY_IN_OBJECT:
        case AFTER_KEY_IN_OBJECT:
            m_State = AT_COMMA_IN_OBJECT;
            READER_THROW("Unexpected comma.");
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected comma.");
        }
    }

    void Reader::processEndArray()
    {
        switch (m_State)
        {
        case AT_START_OF_ARRAY:
        case AT_VALUE_IN_ARRAY:
        case AFTER_VALUE_IN_ARRAY:
            m_State = AT_END_OF_ARRAY;
            break;
        case AT_COMMA_IN_ARRAY:
            m_State = AT_END_OF_ARRAY;
            if (!isEndElementAfterCommaEnabled())
                READER_THROW("Unexpected ']'");
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected ']'");
        }
    }

    void Reader::processEndObject()
    {
        switch (m_State)
        {
        case AT_START_OF_OBJECT:
        case AT_VALUE_IN_OBJECT:
        case AFTER_VALUE_IN_OBJECT:
            m_State = AT_END_OF_OBJECT;
            break;
        case AT_COMMA_IN_OBJECT:
            m_State = AT_END_OF_OBJECT;
            if (!isEndElementAfterCommaEnabled())
                READER_THROW("Unexpected '}'");
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected '}'");
        }
    }

    void Reader::processEndOfStream()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
        case AT_END_OF_DOCUMENT:
            m_State = AT_END_OF_STREAM;
            break;
        case AT_END_OF_STREAM:
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected end of document.");
        }
    }

    void Reader::processStartArray()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
            m_State = AT_VALUE_OF_DOCUMENT;
            break;
        case AT_COLON_IN_OBJECT:
            m_State = AT_VALUE_IN_OBJECT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_VALUE_IN_ARRAY;
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected '['.");
        }
    }

    void Reader::processStartObject()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
            m_State = AT_VALUE_OF_DOCUMENT;
            break;
        case AT_COLON_IN_OBJECT:
            m_State = AT_VALUE_IN_OBJECT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_VALUE_IN_ARRAY;
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected '{'");
        }
    }

    void Reader::processString()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
            m_State = AT_VALUE_OF_DOCUMENT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_VALUE_IN_ARRAY;
            break;
        case AT_START_OF_OBJECT:
        case AT_COMMA_IN_OBJECT:
            m_State = AT_KEY_IN_OBJECT;
            break;
        case AT_COLON_IN_OBJECT:
            m_State = AT_VALUE_IN_OBJECT;
            break;
        default:
            READER_THROW("Unexpected string.");
        }
    }

    void Reader::processValue()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
            m_State = AT_VALUE_OF_DOCUMENT;
            break;
        case AT_START_OF_ARRAY:
        case AT_COMMA_IN_ARRAY:
            m_State = AT_VALUE_IN_ARRAY;
            break;
        case AT_COLON_IN_OBJECT:
            m_State = AT_VALUE_IN_OBJECT;
            break;
        case AT_VALUE_IN_OBJECT:
        case AT_VALUE_IN_ARRAY:
        case AT_START_OF_OBJECT:
        case AT_COMMA_IN_OBJECT:
            m_State = AT_KEY_IN_OBJECT;
            if (isValuesAsKeysEnabled())
                break;
            READER_THROW("Unexpected value: " + m_Tokenizer.token());
        default:
            m_State = UNRECOVERABLE_ERROR;
            READER_THROW("Unexpected value: " + m_Tokenizer.token());
        }
    }

    void Reader::processWhitespace()
    {
        switch (m_State)
        {
        case AT_VALUE_IN_ARRAY:
            m_State = AFTER_VALUE_IN_ARRAY;
            break;
        case AT_KEY_IN_OBJECT:
            m_State = AFTER_KEY_IN_OBJECT;
            break;
        case AT_VALUE_IN_OBJECT:
            m_State = AFTER_VALUE_IN_OBJECT;
            break;
        default:
            break;
        }
    }

    void Reader::skipElement()
    {
        if (m_SkipElementDepth == 0)
        {
            if (!isStartOfStructure(m_Tokenizer.tokenType()))
                return;
            enter();
            m_SkipElementDepth = 1;
        }

        while (m_SkipElementDepth != 0)
        {
            if (!nextTokenImpl())
            {
                switch (m_Tokenizer.tokenType())
                {
                case TokenType::START_ARRAY:
                case TokenType::START_OBJECT:
                    enter();
                    ++m_SkipElementDepth;
                    break;
                case TokenType::END_ARRAY:
                case TokenType::END_OBJECT:
                    leave();
                    --m_SkipElementDepth;
                    break;
                default:
                    break;
                }
            }
        }
    }

    void Reader::skipWhitespace()
    {
        while (m_State != AT_END_OF_STREAM)
        {
            switch (m_Tokenizer.peek().first)
            {
            case TokenType::COMMENT:
            case TokenType::BLOCK_COMMENT:
                if (!isCommentsEnabled())
                    return;
                nextTokenImpl();
                break;
            case TokenType::WHITESPACE:
                nextTokenImpl();
                break;
            default:
                return;
            }
        }
    }

    std::tuple<const char*, const char*, bool> Reader::getValueToken() const
    {
        auto token = m_Tokenizer.rawToken();
        auto isString = false;
        switch (m_Tokenizer.tokenType())
        {
        case TokenType::STRING:
            ++token.first;
            --token.second;
            isString = true;
            break;
        case TokenType::VALUE:
            break;
        case TokenType::BLOCK_STRING:
            token.first += 3;
            token.second -= 3;
            isString = true;
            break;
        default:
            READER_THROW("Current token is not a value.");
        }
        return std::make_tuple(token.first, token.second, isString);
    }

    template<typename T>
    void Reader::readSignedInteger(T& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseInteger(
                get<0>(token), get<1>(token),
                get<2>(token) || isExtendedIntegersEnabled());
        if (!parsedValue.second)
            READER_THROW("Invalid integer");
        value = static_cast<T>(parsedValue.first);
        if (value != parsedValue.first)
            READER_THROW("Overflow error while reading integer value.");
    }

    template<typename T>
    void Reader::readUnsignedInteger(T& value) const
    {
        auto token = getValueToken();
        if (get<0>(token) != get<1>(token) && *get<0>(token) == '-')
            READER_THROW("Attempt to read a signed integer as an unsigned integer.");

        auto parsedValue = parseInteger(get<0>(token), get<1>(token),
                                        get<2>(token) || isExtendedIntegersEnabled());
        if (!parsedValue.second)
            READER_THROW("Invalid integer");
        auto unsignedValue = static_cast<uint64_t>(parsedValue.first);
        value = static_cast<T>(unsignedValue);
        if (value != unsignedValue)
            READER_THROW("Overflow error while reading integer value.");
    }
}
