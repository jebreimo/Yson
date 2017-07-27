//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <stack>
#include "../../Ystring/Escape/Escape.hpp"
#include "../Common/Base64.hpp"
#include "../Common/GetDetailedValueType.hpp"
#include "../Common/GetValueType.hpp"
#include "JsonArrayReader.hpp"
#include "JsonDocumentReader.hpp"
#include "JsonObjectReader.hpp"
#include "../JsonReader.hpp"
#include "JsonReaderException.hpp"
#include "../Common/ParseFloatingPoint.hpp"
#include "../Common/ParseInteger.hpp"

namespace Yson
{
    struct JsonReader::Members
    {
        Members(JsonTokenizer&& tokenizer)
                : tokenizer(std::move(tokenizer))
        {}

        JsonTokenizer tokenizer;
        std::stack<std::pair<JsonScopeReader*, JsonReaderState>> scopes;
        JsonArrayReader arrayReader;
        JsonDocumentReader documentReader;
        JsonObjectReader objectReader;
    };

    JsonReader::JsonReader()
    {}

    JsonReader::JsonReader(std::istream& stream)
            : m_Members(new Members(JsonTokenizer(stream)))
    {
        m_Members->scopes.push({&m_Members->documentReader,
                                JsonReaderState::INITIAL_STATE});
    }

    JsonReader::JsonReader(const std::string& fileName)
            : m_Members(new Members(JsonTokenizer(fileName)))
    {
        m_Members->scopes.push({&m_Members->documentReader,
                                JsonReaderState::INITIAL_STATE});
    }

    JsonReader::JsonReader(const char* buffer, size_t bufferSize)
            : m_Members(new Members(JsonTokenizer(buffer, bufferSize)))
    {
        m_Members->scopes.push({&m_Members->documentReader,
                                JsonReaderState::INITIAL_STATE});
    }

    JsonReader::~JsonReader() = default;

    JsonReader::JsonReader(JsonReader&&) = default;

    JsonReader& JsonReader::operator=(JsonReader&&) = default;

    bool JsonReader::nextDocument()
    {
        auto& scope = m_Members->scopes.top();
        auto result = scope.first->nextDocument(m_Members->tokenizer,
                                                scope.second);
        scope.second = result.first;
        return result.second;
    }

    bool JsonReader::nextKey()
    {
        auto& scope = m_Members->scopes.top();
        auto result = scope.first->nextKey(m_Members->tokenizer,
                                           scope.second);
        scope.second = result.first;
        return result.second;
    }

    bool JsonReader::nextValue()
    {
        auto& scope = m_Members->scopes.top();
        auto result = scope.first->nextValue(m_Members->tokenizer,
                                             scope.second);
        scope.second = result.first;
        return result.second;
    }

    void JsonReader::enter()
    {
        auto state = m_Members->scopes.top().second;
        auto tokenType = m_Members->tokenizer.tokenType();
        if (state == JsonReaderState::AT_VALUE)
        {
            JsonScopeReader* nextReader;
            if (tokenType == JsonTokenType::START_OBJECT)
                nextReader = &m_Members->objectReader;
            else if (tokenType == JsonTokenType::START_ARRAY)
                nextReader = &m_Members->arrayReader;
            else
                JSON_READER_THROW(
                        "There is no object or array to be entered.",
                        m_Members->tokenizer);
            m_Members->scopes.push({nextReader, JsonReaderState::AT_START});
        }
        else
        {
            JSON_READER_THROW("Select a value before calling enter.",
                              m_Members->tokenizer);
        }
    }

    void JsonReader::leave()
    {
        if (m_Members->scopes.size() == 1)
        {
            JSON_READER_THROW(
                    "Cannot call leave() when not inside an array or object",
                    m_Members->tokenizer);
        }
        if (m_Members->scopes.top().second != JsonReaderState::AT_END)
        {
            auto& scope = m_Members->scopes.top();
            while (true)
            {
                auto result = scope.first->nextValue(m_Members->tokenizer,
                                                     scope.second);
                scope.second = result.first;
                if (!result.second)
                    break;
            }
        }
        m_Members->scopes.pop();
        m_Members->scopes.top().second = JsonReaderState::AFTER_VALUE;
    }

    ValueType JsonReader::valueType(bool analyzeStrings) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case JsonTokenType::START_ARRAY:
            return ValueType::ARRAY;
        case JsonTokenType::START_OBJECT:
            return ValueType::OBJECT;
        case JsonTokenType::STRING:
            if (analyzeStrings)
            {
                auto type = getValueType(m_Members->tokenizer.token());
                if (type != ValueType::INVALID)
                    return type;
            }
            return ValueType::STRING;
        case JsonTokenType::VALUE:
            return getValueType(m_Members->tokenizer.token());
        default:
            return ValueType::INVALID;
        }
    }

    DetailedValueType JsonReader::detailedValueType(bool analyzeStrings) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case JsonTokenType::START_ARRAY:
            return DetailedValueType::ARRAY;
        case JsonTokenType::START_OBJECT:
            return DetailedValueType::OBJECT;
        case JsonTokenType::STRING:
            if (analyzeStrings)
            {
                auto token = m_Members->tokenizer.token();
                auto type = getDetailedValueType(token);
                if (type != DetailedValueType::INVALID)
                    return type;
            }
            return DetailedValueType::STRING;
        case JsonTokenType::VALUE:
            return getDetailedValueType(m_Members->tokenizer.token());
        default:
            return DetailedValueType::INVALID;
        }
    }

    bool JsonReader::readNull() const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValue())
        {
            return m_Members->tokenizer.tokenString() == "null";
        }
        return false;
    }

    bool JsonReader::read(bool& value) const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValue())
        {
            if (m_Members->tokenizer.tokenString() == "true")
            {
                value = true;
                return true;
            }
            if (m_Members->tokenizer.tokenString() == "false"
                || m_Members->tokenizer.tokenString() == "null")
            {
                value = false;
                return true;
            }
        }
        return false;
    }

    bool JsonReader::read(int8_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(int16_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(int32_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(int64_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(uint8_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(uint16_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(uint32_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(uint64_t& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(float& value) const
    {
        return readFloatingPoint(value);
    }

    bool JsonReader::read(double& value) const
    {
        return readFloatingPoint(value);
    }

    bool JsonReader::read(long double& value) const
    {
        return readFloatingPoint(value);
    }

    bool JsonReader::read(char& value) const
    {
        assertStateIsKeyOrValue();
        auto tokenType = m_Members->tokenizer.tokenType();
        if (tokenType == JsonTokenType::VALUE)
            return parse(m_Members->tokenizer.token(), value, true);
        if (tokenType == JsonTokenType::STRING)
        {
            auto token = m_Members->tokenizer.token();
            if (token.size() == 1)
            {
                value = token[0];
                return true;
            }
            if (token.size() > 1)
            {
                auto tokenString = m_Members->tokenizer.tokenString();
                if (Ystring::hasEscapedCharacters(tokenString))
                {
                    tokenString = Ystring::unescape(tokenString);
                    if (tokenString.size() == 1)
                    {
                        value = tokenString.front();
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool JsonReader::read(std::string& value) const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValueOrString())
        {
            value = m_Members->tokenizer.tokenString();
            if (Ystring::hasEscapedCharacters(value))
                value = Ystring::unescape(value);
            return true;
        }
        return false;
    }

    bool JsonReader::readBase64(std::vector<char>& value) const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsString())
            return fromBase64(m_Members->tokenizer.token(), value);
        return false;
    }

    bool JsonReader::readBinary(void* buffer, size_t& size)
    {
        auto state = m_Members->scopes.top().second;
        if (state != JsonReaderState::AT_VALUE)
        {
            JSON_READER_THROW("Current token is not a key or a value.",
                              m_Members->tokenizer);
        }
        if (!currentTokenIsString())
            return false;
        return fromBase64(m_Members->tokenizer.token(),
                          static_cast<char*>(buffer), size);
    }

    bool JsonReader::readBinary(std::vector<char>& value)
    {
        return readBase64(value);
    }

    void JsonReader::assertStateIsKeyOrValue() const
    {
        auto state = m_Members->scopes.top().second;
        if (state != JsonReaderState::AT_KEY
            && state != JsonReaderState::AT_VALUE)
        {
            JSON_READER_THROW("Current token is not a key or a value.",
                              m_Members->tokenizer);
        }
    }

    std::string JsonReader::fileName() const
    {
        return m_Members->tokenizer.fileName();
    }

    size_t JsonReader::lineNumber() const
    {
        return m_Members->tokenizer.lineNumber();
    }

    size_t JsonReader::columnNumber() const
    {
        return m_Members->tokenizer.columnNumber();
    }

    bool JsonReader::currentTokenIsValueOrString() const
    {
        auto tokenType = m_Members->tokenizer.tokenType();
        return tokenType == JsonTokenType::VALUE
               || tokenType == JsonTokenType::STRING;
    }

    bool JsonReader::currentTokenIsString() const
    {
        auto tokenType = m_Members->tokenizer.tokenType();
        return tokenType == JsonTokenType::STRING;
    }

    bool JsonReader::currentTokenIsValue() const
    {
        return currentTokenIsValueOrString();
    }

    template <typename T>
    bool JsonReader::readInteger(T& value) const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValue())
            return parse(m_Members->tokenizer.token(), value, true);
        return false;
    }

    template <typename T>
    bool JsonReader::readFloatingPoint(T& value) const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValue())
            return parse(m_Members->tokenizer.token(), value);
        return false;
    }
}
