//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader.hpp"

#include "Yson/ArrayItem.hpp"
#include "Yson/ObjectItem.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/Escape.hpp"
#include "Yson/Common/GetDetailedValueType.hpp"
#include "Yson/Common/GetValueType.hpp"
#include "Yson/Common/IsJavaScriptIdentifier.hpp"
#include "Yson/Common/ParseFloatingPoint.hpp"
#include "Yson/Common/ParseInteger.hpp"
#include "JsonArrayReader.hpp"
#include "JsonDocumentReader.hpp"
#include "JsonObjectReader.hpp"
#include "ThrowJsonReaderException.hpp"

namespace Yson
{
    struct JsonReader::Members
    {
        explicit Members(JsonTokenizer&& tokenizer)
                : tokenizer(std::move(tokenizer))
        {}

        JsonTokenizer tokenizer;
        std::vector<std::pair<JsonScopeReader*, ReaderState>> scopes;
        JsonArrayReader arrayReader;
        JsonDocumentReader documentReader;
        JsonObjectReader objectReader;

        ReaderState& currentState()
        {
            return scopes.back().second;
        }
    };

    JsonReader::JsonReader() = default;

    JsonReader::JsonReader(std::istream& stream)
            : JsonReader(stream, nullptr, 0)
    {}

    JsonReader::JsonReader(const std::string& fileName)
            : m_Members(std::make_unique<Members>(JsonTokenizer(fileName)))
    {
        m_Members->scopes.emplace_back(&m_Members->documentReader,
                                       ReaderState::INITIAL_STATE);
    }

    JsonReader::JsonReader(const char* buffer, size_t bufferSize)
            : m_Members(std::make_unique<Members>(JsonTokenizer(buffer, bufferSize)))
    {
        m_Members->scopes.emplace_back(&m_Members->documentReader,
                                       ReaderState::INITIAL_STATE);
    }

    JsonReader::JsonReader(std::istream& stream,
                           const char* buffer,
                           size_t bufferSize)
            : m_Members(std::make_unique<Members>(JsonTokenizer(stream, buffer, bufferSize)))
    {
        m_Members->scopes.emplace_back(&m_Members->documentReader,
                                       ReaderState::INITIAL_STATE);
    }

    JsonReader::~JsonReader() = default;

    JsonReader::JsonReader(JsonReader&&) noexcept = default;

    JsonReader& JsonReader::operator=(JsonReader&&) noexcept = default;

    bool JsonReader::nextDocument()
    {
        auto& scope = m_Members->scopes.back();
        auto result = scope.first->nextDocument(m_Members->tokenizer,
                                                scope.second);
        scope.second = result.first;
        return result.second;
    }

    bool JsonReader::nextKey()
    {
        auto& scope = m_Members->scopes.back();
        auto result = scope.first->nextKey(m_Members->tokenizer,
                                           scope.second);
        scope.second = result.first;
        return result.second;
    }

    bool JsonReader::nextValue()
    {
        auto& scope = m_Members->scopes.back();
        auto result = scope.first->nextValue(m_Members->tokenizer,
                                             scope.second);
        scope.second = result.first;
        return result.second;
    }

    void JsonReader::enter()
    {
        auto state = m_Members->currentState();
        if (state == ReaderState::AT_VALUE)
        {
            auto tokenType = m_Members->tokenizer.tokenType();
            JsonScopeReader* nextReader;
            if (tokenType == JsonTokenType::START_OBJECT)
                nextReader = &m_Members->objectReader;
            else if (tokenType == JsonTokenType::START_ARRAY)
                nextReader = &m_Members->arrayReader;
            else
                JSON_READER_THROW(
                        "There is no object or array to be entered.",
                        m_Members->tokenizer);
            m_Members->scopes.emplace_back(nextReader, ReaderState::AT_START);
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
        if (m_Members->currentState() != ReaderState::AT_END)
        {
            auto& scope = m_Members->scopes.back();
            while (true)
            {
                auto result = scope.first->nextValue(m_Members->tokenizer,
                                                     scope.second);
                scope.second = result.first;
                if (!result.second)
                    break;
            }
        }
        m_Members->scopes.pop_back();
        m_Members->currentState() = ReaderState::AFTER_VALUE;
    }

    ValueType JsonReader::valueType() const
    {
        return valueType(false);
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
            {
                auto type = getValueType(m_Members->tokenizer.token());
                if (type != ValueType::INVALID
                    || m_Members->currentState() != ReaderState::AT_KEY
                    || !isJavaScriptIdentifier(m_Members->tokenizer.token()))
                {
                    return type;
                }
                return ValueType::STRING;
            }
        default:
            return ValueType::INVALID;
        }
    }

    DetailedValueType JsonReader::detailedValueType() const
    {
        return detailedValueType(false);
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
            {
                auto type = getDetailedValueType(m_Members->tokenizer.token());
                if (type != DetailedValueType::INVALID
                    || m_Members->currentState() != ReaderState::AT_KEY
                    || !isJavaScriptIdentifier(m_Members->tokenizer.token()))
                {
                    return type;
                }
                return DetailedValueType::STRING;
            }
        default:
            return DetailedValueType::INVALID;
        }
    }

    bool JsonReader::readNull() const
    {
        assertStateIsKeyOrValue();
        if (currentTokenIsValue())
        {
            return m_Members->tokenizer.tokenString() ==     "null";
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
            uint8_t uValue;
            if (readInteger(uValue) && uValue <= 1)
            {
                value = uValue == 1;
                return true;
            }
        }
        return false;
    }

    bool JsonReader::read(signed char& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(short& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(int& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(long& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(long long& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(unsigned char& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(unsigned short& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(unsigned int& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(unsigned long& value) const
    {
        return readInteger(value);
    }

    bool JsonReader::read(unsigned long long& value) const
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
                if (hasEscapedCharacters(tokenString))
                {
                    tokenString = unescape(tokenString);
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
            if (hasEscapedCharacters(value))
                value = unescape(value);
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
        auto state = m_Members->currentState();
        if (state != ReaderState::AT_VALUE)
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

    JsonItem JsonReader::readArray() // NOLINT(*-no-recursion)
    {
        std::vector<JsonItem> values;
        const auto& tokenizer = m_Members->tokenizer;
        enter();
        while (true)
        {
            if (!nextValue())
                break;

            auto tType = tokenizer.tokenType();
            if (tType == JsonTokenType::START_OBJECT)
                values.push_back(readObject());
            else if (tType == JsonTokenType::START_ARRAY)
                values.push_back(readArray());
            else
                values.emplace_back(JsonValueItem(tokenizer.tokenString(), tType));
        }
        leave();
        return JsonItem(std::make_shared<ArrayItem>(std::move(values)));
    }

    JsonItem JsonReader::readObject() // NOLINT(*-no-recursion)
    {
        std::deque<std::string> keys;
        std::unordered_map<std::string_view, JsonItem> values;
        const auto& tokenizer = m_Members->tokenizer;
        enter();
        while (true)
        {
            if (!nextKey())
                break;

            auto entry = values.find(tokenizer.token());
            std::string_view key;
            if (entry != values.end())
            {
                key = entry->first;
            }
            else
            {
                keys.emplace_back(tokenizer.token());
                key = keys.back();
            }

            if (!nextValue())
            {
                JSON_READER_THROW("Key without value: " + keys.back(),
                                  tokenizer);
            }

            auto tType = tokenizer.tokenType();
            if (tType == JsonTokenType::START_OBJECT)
                values.insert_or_assign(key, readObject());
            else if (tType == JsonTokenType::START_ARRAY)
                values.insert_or_assign(key, readArray());
            else
                values.insert_or_assign(key, JsonItem(JsonValueItem(tokenizer.tokenString(), tType)));
        }
        leave();
        return JsonItem(std::make_shared<ObjectItem>(std::move(keys), std::move(values)));
    }

    JsonItem JsonReader::readItem()
    {
        auto& tokenizer = m_Members->tokenizer;
        switch (m_Members->currentState())
        {
        case ReaderState::INITIAL_STATE:
        case ReaderState::AT_START:
            if (!nextValue())
                JSON_READER_THROW("Document is empty.", tokenizer);
            [[fallthrough]];
        case ReaderState::AT_VALUE:
        {
            auto tType = tokenizer.tokenType();
            if (tType == JsonTokenType::START_OBJECT)
                return readObject();
            if (tType == JsonTokenType::START_ARRAY)
                return readArray();
            return JsonItem(JsonValueItem(tokenizer.tokenString(), tType));
        }
        case ReaderState::AT_KEY:
            return JsonItem(JsonValueItem(tokenizer.tokenString(),
                                      tokenizer.tokenType()));
        default:
            JSON_READER_THROW("No key or value.", tokenizer);
        }
    }

    void JsonReader::assertStateIsKeyOrValue() const
    {
        auto state = m_Members->currentState();
        if (state != ReaderState::AT_KEY
            && state != ReaderState::AT_VALUE)
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

    ReaderState JsonReader::state() const
    {
        return m_Members->currentState();
    }

    std::string JsonReader::scope() const
    {
        std::string result;
        for (auto& scope : m_Members->scopes)
        {
            if (auto c = scope.first->scopeType())
                result.push_back(c);

        }
        return result;
    }
}
