//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-25
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonReader.hpp"

#include "GetValueType.hpp"
#include "JsonReaderException.hpp"
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
    }

    JsonReader::JsonReader(std::istream& stream, Encoding_t encoding)
        : m_TextReader(new TextReader(stream, encoding, Encoding::UTF_8))
    {}

    JsonReader::JsonReader(const std::string& fileName, Encoding_t encoding)
        : m_TextReader(new TextFileReader(fileName, encoding,
                                          Encoding::UTF_8))
    {}

    bool JsonReader::allowMinorSyntaxErrors() const
    {
        return m_AllowMinorSyntaxErrors;
    }

    JsonReader& JsonReader::setAllowMinorSyntaxErrors(bool value)
    {
        m_AllowMinorSyntaxErrors = value;
        return *this;
    }

    bool JsonReader::allowComments() const
    {
        return m_AllowComments;
    }

    JsonReader& JsonReader::setAllowComments(bool value)
    {
        m_AllowComments = value;
        return *this;
    }

    bool JsonReader::allowExtendedValues() const
    {
        return m_AllowExtendedValues;
    }

    JsonReader& JsonReader::setAllowExtendedValues(bool value)
    {
        m_AllowExtendedValues = value;
        return *this;
    }

    ValueType_t JsonReader::valueType() const
    {
        switch (m_Tokenizer.tokenType())
        {
        case JsonTokenizer::START_ARRAY:
            return ValueType::ARRAY;
        case JsonTokenizer::START_OBJECT:
            return ValueType::OBJECT;
        case JsonTokenizer::STRING:
            return ValueType::STRING;
        case JsonTokenizer::VALUE:
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
            if (m_AllowExtendedValues)
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
        switch (m_State)
        {
        case INITIAL_STATE:
            fillBuffer();
            m_State = START_OF_DOCUMENT;
            break;
        case AFTER_DOCUMENT_VALUE:
            m_State = END_OF_DOCUMENT;
            return false;
        case END_OF_DOCUMENT:
            return false;
        default:
            if (m_EnteredElements.size() > m_StateStack.size())
                return false;
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
            case JsonTokenizer::INVALID_TOKEN:
                YSON_THROW("Invalid token.");
            case JsonTokenizer::START_ARRAY:
                processStartArray();
                break;
            case JsonTokenizer::END_ARRAY:
                processEndArray();
                break;
            case JsonTokenizer::START_OBJECT:
                processStartObject();
                break;
            case JsonTokenizer::END_OBJECT:
                processEndObject();
                break;
            case JsonTokenizer::COLON:
                processColon();
                break;
            case JsonTokenizer::COMMA:
                processComma();
                break;
            case JsonTokenizer::STRING:
                processString();
                break;
            case JsonTokenizer::VALUE:
                processValue();
                break;
            case JsonTokenizer::END_OF_BUFFER:
                if (!fillBuffer() && m_State != END_OF_DOCUMENT)
                    YSON_THROW("Unexpected end of document.");
                filledBuffer = true;
                break;
            case JsonTokenizer::BLOCK_STRING:
                return false;
                break;
            case JsonTokenizer::COMMENT:
                if (!m_AllowComments)
                    YSON_THROW("Invalid token.");
                processWhitespace();
                break;
            case JsonTokenizer::BLOCK_COMMENT:
            {
                if (!m_AllowComments)
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
            case JsonTokenizer::WHITESPACE:
                processWhitespace();
                break;
            case JsonTokenizer::NEWLINE:
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
        switch (m_State)
        {
        case AFTER_OBJECT_ENTRY:
            break;
        case AFTER_OBJECT_START:
            if (m_EnteredElements.size() != m_StateStack.size() - 1)
                YSON_THROW("Call enter() on object before nextKey()");
            skipElement();
            break;
        case AFTER_OBJECT_KEY:
        case AFTER_OBJECT_COLON:
        case AFTER_OBJECT_VALUE:
        case AFTER_OBJECT_COMMA:
        case BEFORE_OBJECT_COMMA:
        case BEFORE_OBJECT_COLON:
        case BEFORE_OBJECT_KEY:
            if (m_EnteredElements.size() != m_StateStack.size())
                YSON_THROW("Call enter() on object before nextKey()");
            break;
        default:
            YSON_THROW("nextKey() can only be called inside an object.");
        }
        while (nextToken())
        {
            switch (m_State)
            {
            case AFTER_OBJECT_START:
            case AFTER_ARRAY_START:
                skipElement();
                break;
            case AFTER_OBJECT_KEY:
                return true;
            case AFTER_OBJECT_ENTRY:
            case AFTER_OBJECT_COLON:
            case AFTER_OBJECT_VALUE:
            case AFTER_OBJECT_COMMA:
            case BEFORE_OBJECT_COLON:
            case BEFORE_OBJECT_COMMA:
            case BEFORE_OBJECT_KEY:
                break;
            default:
                return false;
            }
        }
        return false;
    }

    bool JsonReader::nextValue()
    {
        switch (m_State)
        {
        case INITIAL_STATE:
        case START_OF_DOCUMENT:
        case AFTER_OBJECT_ENTRY:
        case AFTER_ARRAY_ENTRY:
        case AFTER_DOCUMENT_VALUE:
            break;
        case AFTER_ARRAY_START:
        case AFTER_OBJECT_START:
            if (m_EnteredElements.size() != m_StateStack.size() - 1)
                YSON_THROW("Call enter() on element before nextValue()");
            skipElement();
            break;
        case AFTER_OBJECT_KEY:
        case AFTER_OBJECT_COLON:
        case AFTER_OBJECT_VALUE:
        case AFTER_OBJECT_COMMA:
        case AFTER_ARRAY_VALUE:
        case AFTER_ARRAY_COMMA:
        case BEFORE_OBJECT_COMMA:
        case BEFORE_OBJECT_COLON:
        case BEFORE_OBJECT_KEY:
        case BEFORE_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            if (m_EnteredElements.size() != m_StateStack.size())
                YSON_THROW("Call enter() on element before nextValue()");
            break;
        default:
            YSON_THROW("nextValue() can only be called inside an element.");
        }
        while (nextToken())
        {
            switch (m_State)
            {
            case AFTER_OBJECT_START:
            case AFTER_ARRAY_START:
            case AFTER_OBJECT_VALUE:
            case AFTER_ARRAY_VALUE:
            case AFTER_DOCUMENT_VALUE:
                return true;
            case START_OF_DOCUMENT:
            case AFTER_OBJECT_KEY:
            case AFTER_OBJECT_COLON:
            case AFTER_OBJECT_COMMA:
            case AFTER_ARRAY_ENTRY:
            case AFTER_ARRAY_COMMA:
            case BEFORE_OBJECT_COMMA:
            case BEFORE_OBJECT_COLON:
            case BEFORE_OBJECT_KEY:
            case BEFORE_ARRAY_COMMA:
            case BEFORE_ARRAY_VALUE:
                break;
            default:
                return false;
            }
        }
        return false;
    }

    void JsonReader::skipElement()
    {
        if (m_State != AFTER_ARRAY_START && m_State != AFTER_OBJECT_START)
        {
            YSON_THROW("skipElement() must be called at the start of "
                               "an element.");
        }
        enter();
        while (nextToken());
        leave();
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
        if (m_EnteredElements.size() != m_StateStack.size() - 1)
        {
            YSON_THROW("Can't enter child element unless parent has "
                               "also been entered.");
        }
        if (m_State == AFTER_OBJECT_START)
        {
            m_State = AFTER_OBJECT_ENTRY;
            m_EnteredElements.push(ValueType::OBJECT);
        }
        else if (m_State == AFTER_ARRAY_START)
        {
            m_State = AFTER_ARRAY_ENTRY;
            m_EnteredElements.push(ValueType::ARRAY);
        }
        else
        {
            YSON_THROW("There's no array or object to enter.");
        }
    }

    void JsonReader::leave()
    {
        if (m_EnteredElements.empty())
            YSON_THROW("leave() wasn't preceded by enter().");
        if (m_EnteredElements.size() != m_StateStack.size() + 1)
            YSON_THROW("leave() called before the end of the element.");
        m_EnteredElements.pop();
    }

    void JsonReader::processStartArray()
    {
        switch (m_State)
        {
        case START_OF_DOCUMENT:
            m_StateStack.push(END_OF_DOCUMENT);
            m_State = AFTER_ARRAY_START;
            break;
        case AFTER_OBJECT_COLON:
            m_StateStack.push(BEFORE_OBJECT_COMMA);
            m_State = AFTER_ARRAY_START;
            break;
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_ENTRY:
        case AFTER_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            m_StateStack.push(BEFORE_ARRAY_COMMA);
            m_State = AFTER_ARRAY_START;
            break;
        default:
            YSON_THROW("Unexpected '['");
        }
    }

    void JsonReader::processEndArray()
    {
        switch (m_State)
        {
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_VALUE:
        case BEFORE_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        case AFTER_ARRAY_COMMA:
            if (m_AllowMinorSyntaxErrors)
            {
                m_State = m_StateStack.top();
                m_StateStack.pop();
                break;
            }
            // No break here!
        default:
            YSON_THROW("Unexpected ']'");
        }
    }

    void JsonReader::processStartObject()
    {
        switch (m_State)
        {
        case START_OF_DOCUMENT:
            m_StateStack.push(END_OF_DOCUMENT);
            m_State = AFTER_OBJECT_START;
            break;
        case AFTER_OBJECT_COLON:
            m_StateStack.push(BEFORE_OBJECT_COMMA);
            m_State = AFTER_OBJECT_START;
            break;
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_ENTRY:
        case AFTER_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            m_StateStack.push(BEFORE_ARRAY_COMMA);
            m_State = AFTER_OBJECT_START;
            break;
        default:
            YSON_THROW("Unexpected '{'");
        }
    }

    void JsonReader::processEndObject()
    {
        switch (m_State)
        {
        case AFTER_OBJECT_START:
        case AFTER_OBJECT_ENTRY:
        case AFTER_OBJECT_VALUE:
        case BEFORE_OBJECT_KEY:
        case BEFORE_OBJECT_COMMA:
            m_State = m_StateStack.top();
            m_StateStack.pop();
            break;
        case AFTER_OBJECT_COMMA:
            if (m_AllowMinorSyntaxErrors)
            {
                m_State = m_StateStack.top();
                m_StateStack.pop();
                break;
            }
            // No break here!
        default:
            YSON_THROW("Unexpected '}'");
        }
    }

    void JsonReader::processString()
    {
        switch (m_State)
        {
        case START_OF_DOCUMENT:
            m_State = AFTER_DOCUMENT_VALUE;
            break;
        case AFTER_OBJECT_START:
        case AFTER_OBJECT_ENTRY:
        case AFTER_OBJECT_COMMA:
        case BEFORE_OBJECT_KEY:
            m_State = AFTER_OBJECT_KEY;
            break;
        case AFTER_OBJECT_COLON:
            m_State = AFTER_OBJECT_VALUE;
            break;
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_ENTRY:
        case AFTER_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            m_State = AFTER_ARRAY_VALUE;
            break;
        default:
            YSON_THROW("Unexpected string.");
        }
    }

    void JsonReader::processValue()
    {
        switch (m_State)
        {
        case START_OF_DOCUMENT:
            m_State = AFTER_DOCUMENT_VALUE;
            break;
        case AFTER_OBJECT_COLON:
            m_State = AFTER_OBJECT_VALUE;
            break;
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_ENTRY:
        case AFTER_ARRAY_COMMA:
        case BEFORE_ARRAY_VALUE:
            m_State = AFTER_ARRAY_VALUE;
            break;
        default:
            YSON_THROW("Unexpected value.");
        }
    }

    void JsonReader::processColon()
    {
        switch (m_State)
        {
        case AFTER_OBJECT_KEY:
        case BEFORE_OBJECT_COLON:
            m_State = AFTER_OBJECT_COLON;
            break;
        default:
            YSON_THROW("Unexpected colon.");
        }
    }

    void JsonReader::processComma()
    {
        switch (m_State)
        {
        case AFTER_OBJECT_VALUE:
        case BEFORE_OBJECT_COMMA:
            m_State = AFTER_OBJECT_COMMA;
            break;
        case AFTER_ARRAY_VALUE:
        case BEFORE_ARRAY_COMMA:
            m_State = AFTER_ARRAY_COMMA;
            break;
        default:
            YSON_THROW("Unexpected comma.");
        }
    }

    void JsonReader::read(std::string& value) const
    {
        if (m_Tokenizer.tokenType() != JsonTokenizer::STRING)
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
        if (m_Tokenizer.tokenType() != JsonTokenizer::VALUE)
            YSON_THROW("Current token is not an integer.");
        auto token = m_Tokenizer.rawToken();
        auto parsedValue = parseInteger(token.first, token.second,
                                        m_AllowExtendedValues);
        if (!parsedValue.second)
            YSON_THROW("Invalid integer");
        value = static_cast<T>(parsedValue.first);
        if (value != parsedValue.first)
            YSON_THROW("Overflow error while reading integer value.");
    }

    template<typename T>
    void JsonReader::readUnsignedInteger(T& value) const
    {
        if (m_Tokenizer.tokenType() != JsonTokenizer::VALUE)
            YSON_THROW("Current token is not an integer.");
        auto token = m_Tokenizer.rawToken();
        if (token.first != token.second && *token.first == '-')
        {
            YSON_THROW("Attempt to read a signed integer as "
                               "an unsigned integer.");
        }
        auto parsedValue = parseInteger(token.first, token.second,
                                        m_AllowExtendedValues);
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

    void JsonReader::processWhitespace()
    {
        switch (m_State)
        {
        case AFTER_OBJECT_START:
        case AFTER_OBJECT_ENTRY:
            m_State = BEFORE_OBJECT_KEY;
            break;
        case AFTER_OBJECT_KEY:
            m_State = BEFORE_OBJECT_COLON;
            break;
        case AFTER_OBJECT_VALUE:
            m_State = BEFORE_OBJECT_COMMA;
            break;
        case AFTER_ARRAY_START:
        case AFTER_ARRAY_ENTRY:
            m_State = BEFORE_ARRAY_VALUE;
            break;
        case AFTER_ARRAY_VALUE:
            m_State = BEFORE_ARRAY_COMMA;
            break;
        default:
            break;
        }
    }
}
