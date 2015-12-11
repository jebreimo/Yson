//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonReader.hpp"

#include "GetValueType.hpp"
#include "ParseDouble.hpp"
#include "ParseInteger.hpp"
#include "TextFileReader.hpp"

namespace Yson
{
    using namespace Ystring;
    const size_t CHUNK_SIZE = 16 * 1024;

    #define YSON_THROW(msg) \
        throw JsonReaderException((msg), __FILE__, __LINE__, __FUNCTION__, \
                                  lineNumber(), columnNumber())

    namespace {
        std::pair<size_t, size_t> countRowsAndColumns(
                std::pair<const char*, const char*> token)
        {
            size_t rows = 0, cols = 0;
            for (auto it = token.first; it != token.second; ++it)
            {
                if (*it == '\n')
                {
                    ++rows;
                    cols = 1;
                }
                else if (*it == '\r')
                {
                    cols = 1;
                }
                else
                {
                    ++cols;
                }
            }
            return std::make_pair(rows, cols);
        }

        bool equalsNull(const std::pair<const char*, const char*>& token)
        {
            return token.second - token.first == 4
                   && std::equal(token.first, token.second, "null");
        }

        bool equalsTrue(const std::pair<const char*, const char*>& token)
        {
            return token.second - token.first == 4
                   && std::equal(token.first, token.second, "true");
        }

        bool equalsFalse(const std::pair<const char*, const char*>& token)
        {
            return token.second - token.first == 5
                   && std::equal(token.first, token.second, "false");
        }

        bool isSubElement(JsonTokenType_t tokenType)
        {
            return tokenType == JsonTokenType::START_ARRAY
                   || tokenType == JsonTokenType::START_OBJECT;
        }
    }

    JsonReader::JsonReader(std::istream& stream, Encoding_t encoding)
        : m_TextReader(new TextReader(stream, encoding, Encoding::UTF_8))
    {}

    JsonReader::JsonReader(const std::string& fileName, Encoding_t encoding)
        : m_TextReader(new TextFileReader(fileName, encoding,
                                          Encoding::UTF_8))
    {}

    void JsonReader::read(std::string& value) const
    {
        if (m_Tokenizer.tokenType() != JsonTokenType::STRING)
            YSON_THROW("Current token is not a string.");
        auto token = m_Tokenizer.rawToken();
        value.assign(token.first + 1, token.second - 1);
    }

    void JsonReader::read(int32_t& value) const
    {
        readSignedInteger(value);
    }

    template<typename T>
    void JsonReader::readSignedInteger(T& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseInteger(token.first, token.second,
                                        isExtendedIntegersEnabled());
        if (!parsedValue.second)
            YSON_THROW("Invalid integer");
        value = static_cast<T>(parsedValue.first);
        if (value != parsedValue.first)
            YSON_THROW("Overflow error while reading integer value.");
    }

    template<typename T>
    void JsonReader::readUnsignedInteger(T& value) const
    {
        auto token = getValueToken();
        if (token.first != token.second && *token.first == '-')
        {
            YSON_THROW("Attempt to read a signed integer as "
                               "an unsigned integer.");
        }
        auto parsedValue = parseInteger(token.first, token.second,
                                        isExtendedIntegersEnabled());
        if (!parsedValue.second)
            YSON_THROW("Invalid integer");
        auto unsignedValue = static_cast<uint64_t>(parsedValue.first);
        value = static_cast<T>(unsignedValue);
        if (value != unsignedValue)
            YSON_THROW("Overflow error while reading integer value.");
    }

    void JsonReader::read(int64_t& value) const
    {
        readSignedInteger(value);
    }

    void JsonReader::read(uint64_t& value) const
    {
        readUnsignedInteger(value);
    }

    void JsonReader::read(double& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseDouble(token.first, token.second);
        if (!parsedValue.second)
            YSON_THROW("Invalid floating point value");
        value = parsedValue.first;
    }

    void JsonReader::read(float& value) const
    {
        auto token = getValueToken();
        auto parsedValue = parseFloat(token.first, token.second);
        if (!parsedValue.second)
            YSON_THROW("Invalid floating point value");
        value = parsedValue.first;
    }

    void JsonReader::read(bool& value) const
    {
        auto token = getValueToken();
        if (equalsTrue(token))
            value = true;
        else if (equalsFalse(token))
            value = false;
        else
            YSON_THROW("Invalid boolean value");
    }

    std::pair<const char*, const char*> JsonReader::getValueToken() const
    {
        auto token = m_Tokenizer.rawToken();
        if (m_Tokenizer.tokenType() == JsonTokenType::STRING)
        {
            ++token.first;
            --token.second;
        }
        else if (m_Tokenizer.tokenType() != JsonTokenType::VALUE)
        {
            YSON_THROW("Current token is not a value.");
        }
        return token;
    }

    ValueType_t JsonReader::valueType() const
    {
        switch (m_Tokenizer.tokenType())
        {
        case JsonTokenType::START_ARRAY:
            return ValueType::ARRAY;
        case JsonTokenType::START_OBJECT:
            return ValueType::OBJECT;
        case JsonTokenType::STRING:
            return ValueType::STRING;
        case JsonTokenType::VALUE:
            break;
        default:
            YSON_THROW("Current token has no value type.");
        }

        auto valueType = getValueType(m_Tokenizer.rawToken());
        switch (valueType)
        {
        case ValueType::BIN_INTEGER:
        case ValueType::OCT_INTEGER:
        case ValueType::HEX_INTEGER:
            if (isExtendedIntegersEnabled())
                return ValueType::INTEGER;
            // No break here!
        case ValueType::INVALID:
            YSON_THROW("Invalid value.");
        default:
            return valueType;
        }
    }

    bool JsonReader::nextToken()
    {
        if (m_SkipElementDepth != 0 && m_State != UNRECOVERABLE_ERROR)
            skipElement();

        switch (m_State)
        {
        case AT_VALUE_OF_DOCUMENT:
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_IN_OBJECT:
            if (isSubElement(m_Tokenizer.tokenType()))
                skipElement();
            break;
        default:
            break;
        }

        return nextTokenImpl();
    }

    bool JsonReader::nextTokenImpl()
    {
        switch (m_State)
        {
        case UNRECOVERABLE_ERROR:
            YSON_THROW("Can't continue reading the current stream.");
        case INITIAL_STATE:
            fillBuffer();
            m_State = AT_START_OF_DOCUMENT;
            break;
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_IN_OBJECT:
            if (isSubElement(m_Tokenizer.tokenType()))
                return false;
            break;
        case AT_VALUE_OF_DOCUMENT:
            if (isSubElement(m_Tokenizer.tokenType()))
                return false;
            m_State = AT_END_OF_DOCUMENT;
            return false;
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
        case AT_END_OF_BUFFER:
        case AT_END_OF_DOCUMENT:
            return false;
        default:
            break;
        }

        bool filledBuffer;

        do
        {
            filledBuffer = false;

            m_Tokenizer.next();
            auto tok = m_Tokenizer.rawToken();
            auto nextColumnNumber = m_ColumnNumber + (tok.second - tok.first);

            switch (m_Tokenizer.tokenType())
            {
            case JsonTokenType::INVALID_TOKEN:
                YSON_THROW("Invalid token.");
            case JsonTokenType::START_ARRAY:
                processStartArray();
                break;
            case JsonTokenType::END_ARRAY:
                processEndArray();
                break;
            case JsonTokenType::START_OBJECT:
                processStartObject();
                break;
            case JsonTokenType::END_OBJECT:
                processEndObject();
                break;
            case JsonTokenType::COLON:
                processColon();
                break;
            case JsonTokenType::COMMA:
                processComma();
                break;
            case JsonTokenType::STRING:
                processString();
                break;
            case JsonTokenType::VALUE:
                processValue();
                break;
            case JsonTokenType::END_OF_BUFFER:
                if (fillBuffer())
                    filledBuffer = true;
                else
                    processEndOfBuffer();
                break;
            case JsonTokenType::BLOCK_STRING:
                return false;
                break;
            case JsonTokenType::COMMENT:
                if (!isCommentsEnabled())
                    YSON_THROW("Invalid token.");
                processWhitespace();
                break;
            case JsonTokenType::BLOCK_COMMENT:
            {
                if (!isCommentsEnabled())
                    YSON_THROW("Invalid token.");
                processWhitespace();
                auto lineCol = countRowsAndColumns(m_Tokenizer.rawToken());
                if (lineCol.first)
                {
                    m_LineNumber += lineCol.first;
                    m_ColumnNumber = lineCol.second;
                }
                m_ColumnNumber += lineCol.second;
                break;
            }
            case JsonTokenType::WHITESPACE:
                processWhitespace();
                break;
            case JsonTokenType::NEWLINE:
                processWhitespace();
                ++m_LineNumber;
                nextColumnNumber = 1;
                break;
            }
            m_ColumnNumber = nextColumnNumber;
        } while (filledBuffer);
        return true;
    }

    bool JsonReader::nextKey()
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
            YSON_THROW("nextKey() can only be called inside an object.");
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

    bool JsonReader::nextValue()
    {
        if (m_SkipElementDepth != 0 && m_State != UNRECOVERABLE_ERROR)
            skipElement();

        switch (m_State)
        {
        case AT_END_OF_BUFFER:
        case AT_END_OF_DOCUMENT:
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
            return false;
        case AT_VALUE_IN_OBJECT:
        case AT_VALUE_IN_ARRAY:
        case AT_VALUE_OF_DOCUMENT:
            if (isSubElement(m_Tokenizer.tokenType()))
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

    void JsonReader::skipElement()
    {
        if (m_SkipElementDepth == 0)
        {
            if (!isSubElement(m_Tokenizer.tokenType()))
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
                case JsonTokenType::START_ARRAY:
                case JsonTokenType::START_OBJECT:
                    enter();
                    ++m_SkipElementDepth;
                    break;
                case JsonTokenType::END_ARRAY:
                case JsonTokenType::END_OBJECT:
                    leave();
                    --m_SkipElementDepth;
                    break;
                default:
                    break;
                }
            }
        }
    }

    size_t JsonReader::lineNumber() const
    {
        return m_LineNumber;
    }

    size_t JsonReader::columnNumber() const
    {
        return m_ColumnNumber;
    }

    bool JsonReader::fillBuffer()
    {
        auto initialSize = m_Buffer.size();
        m_Buffer.clear();
        if (!m_TextReader->read(m_Buffer, CHUNK_SIZE) && initialSize == 0)
            return false;
        m_Tokenizer.setBuffer(m_Buffer.data(), m_Buffer.size());
        return true;
    }

    void JsonReader::enter()
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
            YSON_THROW("There's no array or object to enter.");
        }
        if (m_Tokenizer.tokenType() == JsonTokenType::START_OBJECT)
            m_State = AT_START_OF_OBJECT;
        else if (m_Tokenizer.tokenType() == JsonTokenType::START_ARRAY)
            m_State = AT_START_OF_ARRAY;
        else if (isEnterNullEnabled() && isNull())
            m_State = AT_END_OF_NULL;
        else
            YSON_THROW("Only arrays and objects can be entered.");
        m_StateStack.push(pushState);
    }

    void JsonReader::leave()
    {
        switch (m_State)
        {
        case INITIAL_STATE:
        case AT_START_OF_DOCUMENT:
        case AT_VALUE_OF_DOCUMENT:
        case AT_END_OF_DOCUMENT:
        case AT_END_OF_BUFFER:
            YSON_THROW("leave() wasn't preceded by enter().");
        case AT_END_OF_ARRAY:
        case AT_END_OF_OBJECT:
        case AT_END_OF_NULL:
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        default:
            while (nextToken())
                ;
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        }
    }

    void JsonReader::processStartArray()
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
            YSON_THROW("Unexpected '['.");
        }
    }

    void JsonReader::processEndArray()
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
                YSON_THROW("Unexpected ']'");
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected ']'");
        }
    }

    void JsonReader::processStartObject()
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
            YSON_THROW("Unexpected '{'");
        }
    }

    void JsonReader::processEndObject()
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
                YSON_THROW("Unexpected '}'");
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected '}'");
        }
    }

    void JsonReader::processString()
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
            YSON_THROW("Unexpected string.");
        }
    }

    void JsonReader::processValue()
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
            YSON_THROW("Unexpected value: " + m_Tokenizer.token());
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected value: " + m_Tokenizer.token());
        }
    }

    void JsonReader::processColon()
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
            YSON_THROW("Unexpected colon.");
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected colon.");
        }
    }

    void JsonReader::processComma()
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
            YSON_THROW("Unexpected comma.");
        case AT_START_OF_OBJECT:
        case AT_COMMA_IN_OBJECT:
        case AT_KEY_IN_OBJECT:
        case AFTER_KEY_IN_OBJECT:
            m_State = AT_COMMA_IN_OBJECT;
            YSON_THROW("Unexpected comma.");
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected comma.");
        }
    }

    void JsonReader::processWhitespace()
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

    bool JsonReader::isStringsAsValuesEnabled() const
    {
        return languageExtension(STRINGS_AS_VALUES);
    }

    JsonReader& JsonReader::setStringsAsValuesEnabled(bool value)
    {
        return setLanguageExtension(STRINGS_AS_VALUES, value);
    }

    bool JsonReader::isValuesAsStringsEnabled() const
    {
        return languageExtension(VALUES_AS_STRINGS);
    }

    JsonReader& JsonReader::setValuesAsStringsEnabled(bool value)
    {
        return setLanguageExtension(VALUES_AS_STRINGS, value);
    }

    bool JsonReader::isEndElementAfterCommaEnabled() const
    {
        return languageExtension(END_ELEMENT_AFTER_COMMA);
    }

    JsonReader& JsonReader::setEndElementAfterCommaEnabled(bool value)
    {
        return setLanguageExtension(END_ELEMENT_AFTER_COMMA, value);
    }

    bool JsonReader::isCommentsEnabled() const
    {
        return languageExtension(COMMENTS);
    }

    JsonReader& JsonReader::setCommentsEnabled(bool value)
    {
        return setLanguageExtension(COMMENTS, value);
    }

    bool JsonReader::isEnterNullEnabled() const
    {
        return languageExtension(ENTER_NULL);
    }

    JsonReader& JsonReader::setEnterNullEnabled(bool value)
    {
        return setLanguageExtension(ENTER_NULL, value);
    }

    bool JsonReader::isValuesAsKeysEnabled() const
    {
        return languageExtension(VALUES_AS_KEYS);
    }

    JsonReader& JsonReader::setValuesAsKeysEnabled(bool value)
    {
        return setLanguageExtension(VALUES_AS_KEYS, value);
    }

    bool JsonReader::isExtendedIntegersEnabled() const
    {
        return languageExtension(EXTENDED_INTEGERS);
    }

    JsonReader& JsonReader::setExtendedIntegersEnabled(bool value)
    {
        return setLanguageExtension(EXTENDED_INTEGERS, value);
    }

    int JsonReader::languageExtensions() const
    {
        return m_LanguagExtentions;
    }

    JsonReader& JsonReader::setLanguageExtensions(int value)
    {
        m_LanguagExtentions = value;
        return *this;
    }

    bool JsonReader::isNull() const
    {
        auto token = getValueToken();
        return equalsNull(token);
    }

    bool JsonReader::languageExtension(
            JsonReader::LanguageExtensions ext) const
    {
        return (m_LanguagExtentions & ext) != 0;
    }

    JsonReader& JsonReader::setLanguageExtension(
            JsonReader::LanguageExtensions ext, bool value)
    {
        if (value)
            m_LanguagExtentions |= ext;
        else
            m_LanguagExtentions &= ~ext;
        return *this;
    }

    void JsonReader::read(uint32_t& value) const
    {
        readUnsignedInteger(value);
    }

    void JsonReader::read(int8_t& value) const
    {
        readSignedInteger(value);
    }

    void JsonReader::read(int16_t& value) const
    {
        readSignedInteger(value);
    }

    void JsonReader::read(uint8_t& value) const
    {
        readUnsignedInteger(value);
    }

    void JsonReader::read(uint16_t& value) const
    {
        readUnsignedInteger(value);
    }

    bool JsonReader::nextDocument()
    {
        while (m_State != AT_END_OF_DOCUMENT)
        {
            switch (m_State)
            {
            case AT_END_OF_BUFFER:
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
        return true;
    }

    void JsonReader::processEndOfBuffer()
    {
        switch (m_State)
        {
        case AT_START_OF_DOCUMENT:
        case AT_END_OF_DOCUMENT:
            m_State = AT_END_OF_BUFFER;
            break;
        case AT_END_OF_BUFFER:
            break;
        default:
            m_State = UNRECOVERABLE_ERROR;
            YSON_THROW("Unexpected end of document.");
        }
    }

    JsonTokenType_t JsonReader::tokenType() const
    {
        return m_Tokenizer.tokenType();
    }
}
