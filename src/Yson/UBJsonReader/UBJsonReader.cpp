//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/UBJsonReader.hpp"

#include "Yson/ArrayItem.hpp"
#include "Yson/ObjectItem.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/GetDetailedValueType.hpp"
#include "Yson/Common/GetValueType.hpp"
#include "UBJsonArrayReader.hpp"
#include "UBJsonDocumentReader.hpp"
#include "UBJsonObjectReader.hpp"
#include "UBJsonOptimizedArrayReader.hpp"
#include "UBJsonOptimizedObjectReader.hpp"
#include "UBJsonReaderUtilities.hpp"

namespace Yson
{
    namespace
    {
        template <typename T>
        bool readOptimizedArrayToString(UBJsonReader& reader,
                                        std::string& buffer)
        {
            size_t size = 0;
            T* ptr = nullptr;
            if (reader.readOptimizedArray(ptr, size))
            {
                buffer.resize(size);
                ptr = reinterpret_cast<T*>(buffer.data());
                if (reader.readOptimizedArray(ptr, size))
                    return true;
            }

            return false;
        }
    }

    struct UBJsonReader::Scope
    {
        UBJsonScopeReader* reader = nullptr;
        UBJsonReaderState state;
    };

    struct UBJsonReader::Members
    {
        explicit Members(UBJsonTokenizer&& tokenizer)
                : tokenizer(std::move(tokenizer))
        {
            scopes.reserve(50);
        }

        UBJsonTokenizer tokenizer;
        std::vector<Scope> scopes;
        UBJsonArrayReader arrayReader;
        UBJsonDocumentReader documentReader;
        UBJsonObjectReader objectReader;
        UBJsonOptimizedArrayReader optimizedArrayReader;
        UBJsonOptimizedObjectReader optimizedObjectReader;
    };

    UBJsonReader::UBJsonReader() = default;

    UBJsonReader::UBJsonReader(std::istream& stream)
        : UBJsonReader(std::make_unique<Members>(UBJsonTokenizer(stream)))
    {}

    UBJsonReader::UBJsonReader(const std::filesystem::path& fileName)
        : UBJsonReader(std::make_unique<Members>(UBJsonTokenizer(fileName)))
    {}

    UBJsonReader::UBJsonReader(const char* buffer, size_t bufferSize)
        : UBJsonReader(std::make_unique<Members>(UBJsonTokenizer(buffer, bufferSize)))
    {}

    UBJsonReader::UBJsonReader(std::istream& stream, const char* buffer,
                               size_t bufferSize)
        : UBJsonReader(std::make_unique<Members>(UBJsonTokenizer(stream, buffer, bufferSize)))
    {}

    UBJsonReader::UBJsonReader(std::unique_ptr<Members> members)
        : m_Members(std::move(members))
    {
        m_Members->scopes.push_back({&m_Members->documentReader,
                                     UBJsonReaderState()});
    }

    UBJsonReader::UBJsonReader(UBJsonReader&&) noexcept = default;

    UBJsonReader::~UBJsonReader() = default;

    UBJsonReader& UBJsonReader::operator=(UBJsonReader&&) noexcept = default;

    bool UBJsonReader::nextValue()
    {
        auto& scope = currentScope();
        return scope.reader->nextValue(m_Members->tokenizer, scope.state);
    }

    bool UBJsonReader::nextKey()
    {
        auto& scope = currentScope();
        return scope.reader->nextKey(m_Members->tokenizer, scope.state);
    }

    bool UBJsonReader::nextDocument()
    {
        auto& scope = currentScope();
        return scope.reader->nextDocument(m_Members->tokenizer, scope.state);
    }

    void UBJsonReader::enter()
    {
        auto state = currentScope().state.state;
        if (state == ReaderState::AT_VALUE)
        {
            auto options = currentScope().state.options;
            auto tokenType = m_Members->tokenizer.tokenType();
            if (tokenType == UBJsonTokenType::START_OBJECT_TOKEN)
                m_Members->scopes.push_back(
                        {&m_Members->objectReader,
                         UBJsonReaderState(ReaderState::AT_START, options)});
            else if (tokenType == UBJsonTokenType::START_ARRAY_TOKEN)
                m_Members->scopes.push_back(
                        {&m_Members->arrayReader,
                         UBJsonReaderState(ReaderState::AT_START, options)});
            else if (tokenType == UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN)
                m_Members->scopes.push_back(
                        {&m_Members->optimizedArrayReader,
                         makeState(m_Members->tokenizer, options)});
            else if (tokenType == UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN)
                m_Members->scopes.push_back(
                        {&m_Members->optimizedObjectReader,
                         makeState(m_Members->tokenizer, options)});
            else
                UBJSON_READER_THROW(
                        "There is no object or array to be entered.",
                        m_Members->tokenizer);
        }
        else
        {
            UBJSON_READER_THROW("Select a value before calling enter.",
                                m_Members->tokenizer);
        }
    }

    void UBJsonReader::leave()
    {
        if (m_Members->scopes.size() == 1)
        {
            UBJSON_READER_THROW(
                    "Cannot call leave() when not inside an array or object",
                    m_Members->tokenizer);
        }
        auto& state = currentScope().state;
        if (state.state != ReaderState::AT_END)
        {
            auto scopeReader = currentScope().reader;
            while (scopeReader->nextValue(m_Members->tokenizer, state))
            {}
        }
        m_Members->scopes.pop_back();
        currentScope().state.state = ReaderState::AFTER_VALUE;
    }

    ValueType UBJsonReader::valueType() const
    {
        return valueType(false);
    }

    ValueType UBJsonReader::valueType(bool analyzeStrings) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case UBJsonTokenType::UNKNOWN_TOKEN:
            return ValueType::UNKNOWN;
        case UBJsonTokenType::NULL_TOKEN:
            return ValueType::NULL_VALUE;
        case UBJsonTokenType::TRUE_TOKEN:
        case UBJsonTokenType::FALSE_TOKEN:
            return ValueType::BOOLEAN;
        case UBJsonTokenType::INT8_TOKEN:
        case UBJsonTokenType::UINT8_TOKEN:
        case UBJsonTokenType::INT16_TOKEN:
        case UBJsonTokenType::INT32_TOKEN:
        case UBJsonTokenType::INT64_TOKEN:
        case UBJsonTokenType::CHAR_TOKEN:
            return ValueType::INTEGER;
        case UBJsonTokenType::FLOAT32_TOKEN:
        case UBJsonTokenType::FLOAT64_TOKEN:
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
            return ValueType::FLOAT;
        case UBJsonTokenType::STRING_TOKEN:
            if (analyzeStrings)
            {
                auto type = getValueType(m_Members->tokenizer.token());
                if (type != ValueType::INVALID)
                    return type;
            }
            return ValueType::STRING;
        case UBJsonTokenType::START_OBJECT_TOKEN:
        case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
            return ValueType::OBJECT;
        case UBJsonTokenType::START_ARRAY_TOKEN:
        case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
            return ValueType::ARRAY;
        default:
            UBJSON_READER_THROW("Invalid token.", m_Members->tokenizer);
        }
    }

    DetailedValueType UBJsonReader::detailedValueType() const
    {
        return detailedValueType(false);
    }

    DetailedValueType UBJsonReader::detailedValueType(
            bool analyzeStrings) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case UBJsonTokenType::UNKNOWN_TOKEN:
            return DetailedValueType::UNKNOWN;
        case UBJsonTokenType::NULL_TOKEN:
            return DetailedValueType::NULL_VALUE;
        case UBJsonTokenType::TRUE_TOKEN:
        case UBJsonTokenType::FALSE_TOKEN:
            return DetailedValueType::BOOLEAN;
        case UBJsonTokenType::INT8_TOKEN:
            return DetailedValueType::UINT_7;
        case UBJsonTokenType::UINT8_TOKEN:
            return DetailedValueType::UINT_8;
        case UBJsonTokenType::INT16_TOKEN:
            return DetailedValueType::UINT_15;
        case UBJsonTokenType::INT32_TOKEN:
            return DetailedValueType::UINT_31;
        case UBJsonTokenType::INT64_TOKEN:
            return DetailedValueType::UINT_63;
        case UBJsonTokenType::CHAR_TOKEN:
            return DetailedValueType::CHAR;
        case UBJsonTokenType::FLOAT32_TOKEN:
            return DetailedValueType::FLOAT_32;
        case UBJsonTokenType::FLOAT64_TOKEN:
            return DetailedValueType::FLOAT_64;
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
            return DetailedValueType::HIGH_PRECISION_NUMBER;
        case UBJsonTokenType::STRING_TOKEN:
            if (analyzeStrings)
            {
                auto type = getDetailedValueType(
                        m_Members->tokenizer.token());
                if (type != DetailedValueType::INVALID)
                    return type;
            }
            return DetailedValueType::STRING;
        case UBJsonTokenType::START_OBJECT_TOKEN:
        case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
            return DetailedValueType::OBJECT;
        case UBJsonTokenType::START_ARRAY_TOKEN:
        case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
            return DetailedValueType::ARRAY;
        default:
            UBJSON_READER_THROW("Invalid token.", m_Members->tokenizer);
        }
    }

    bool UBJsonReader::isOptimizedArray() const
    {
        auto state = currentScope().state.state;
        auto tokType = m_Members->tokenizer.tokenType();
        return state != ReaderState::AT_VALUE
               && tokType == UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN;
    }

    std::pair<size_t, DetailedValueType>
    UBJsonReader::optimizedArrayProperties() const
    {
        if (!isOptimizedArray())
            return {0, DetailedValueType::UNKNOWN};

        auto size = m_Members->tokenizer.contentSize();
        switch (m_Members->tokenizer.contentType())
        {
        case UBJsonTokenType::NULL_TOKEN:
            return {size, DetailedValueType::NULL_VALUE};
        case UBJsonTokenType::TRUE_TOKEN:
        case UBJsonTokenType::FALSE_TOKEN:
            return {size, DetailedValueType::BOOLEAN};
        case UBJsonTokenType::INT8_TOKEN:
            return {size, DetailedValueType::UINT_7};
        case UBJsonTokenType::UINT8_TOKEN:
            return {size, DetailedValueType::UINT_8};
        case UBJsonTokenType::INT16_TOKEN:
            return {size, DetailedValueType::UINT_15};
        case UBJsonTokenType::INT32_TOKEN:
            return {size, DetailedValueType::UINT_31};
        case UBJsonTokenType::INT64_TOKEN:
            return {size, DetailedValueType::UINT_63};
        case UBJsonTokenType::CHAR_TOKEN:
            return {size, DetailedValueType::CHAR};
        case UBJsonTokenType::FLOAT32_TOKEN:
            return {size, DetailedValueType::FLOAT_32};
        case UBJsonTokenType::FLOAT64_TOKEN:
            return {size, DetailedValueType::FLOAT_64};
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
            return {size, DetailedValueType::HIGH_PRECISION_NUMBER};
        case UBJsonTokenType::STRING_TOKEN:
            return {size, DetailedValueType::STRING};
        case UBJsonTokenType::START_OBJECT_TOKEN:
            return {size, DetailedValueType::OBJECT};
        case UBJsonTokenType::START_ARRAY_TOKEN:
            return {size, DetailedValueType::ARRAY};
        default:
            return {size, DetailedValueType::UNKNOWN};
        }
    }

    bool UBJsonReader::readNull() const
    {
        assertStateIsKeyOrValue();
        return m_Members->tokenizer.tokenType() == UBJsonTokenType::NULL_TOKEN;
    }

    bool UBJsonReader::read(bool& value) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case UBJsonTokenType::TRUE_TOKEN:
            value = true;
            return true;
        case UBJsonTokenType::FALSE_TOKEN:
            value = false;
            return true;
        default:
            return false;
        }
    }

    bool UBJsonReader::read(signed char& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(short& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(int& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(long& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(long long& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(unsigned char& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(unsigned short& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(unsigned int& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(unsigned long& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(unsigned long long& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(float& value) const
    {
        assertStateIsKeyOrValue();
        return setFloatingPointValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(double& value) const
    {
        assertStateIsKeyOrValue();
        return setFloatingPointValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(long double& value) const
    {
        assertStateIsKeyOrValue();
        return setFloatingPointValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(char& value) const
    {
        assertStateIsKeyOrValue();
        return setIntegerValue(value, m_Members->tokenizer);
    }

    bool UBJsonReader::read(std::string& value) const
    {
        assertStateIsKeyOrValue();
        switch (m_Members->tokenizer.tokenType())
        {
        case UBJsonTokenType::STRING_TOKEN:
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
        case UBJsonTokenType::CHAR_TOKEN:
            {
                auto token = m_Members->tokenizer.token();
                value.assign(token.data(), token.size());
                return true;
            }
        default:
            return false;
        }
    }

    bool UBJsonReader::readOptimizedArray(int8_t* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::INT8_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(int16_t* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::INT16_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(int32_t* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::INT32_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(int64_t* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::INT64_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(uint8_t* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::UINT8_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(float* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::FLOAT32_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(double* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::FLOAT64_TOKEN);
    }

    bool UBJsonReader::readOptimizedArray(char* buffer, size_t& size)
    {
        return readOptimizedArrayImpl(buffer, size,
                                      UBJsonTokenType::CHAR_TOKEN);
    }

    bool UBJsonReader::readBinary(void* buffer, size_t& size)
    {
        return readOptimizedArray(static_cast<uint8_t*>(buffer), size);
    }

    bool UBJsonReader::readBinary(std::vector<char>& value)
    {
        size_t size;
        if (readBinary(nullptr, size))
        {
            value.resize(size);
            return readBinary(value.data(), size);
        }
        return false;
    }

    bool UBJsonReader::readBase64(std::vector<char>& value) const
    {
        assertStateIsKeyOrValue();
        if (m_Members->tokenizer.tokenType() == UBJsonTokenType::STRING_TOKEN)
            return fromBase64(m_Members->tokenizer.token(), value);

        return false;
    }

    JsonItem UBJsonReader::readItem()
    {
        auto& tokenizer = m_Members->tokenizer;
        switch (currentScope().state.state)
        {
        case ReaderState::INITIAL_STATE:
        case ReaderState::AT_START:
            if (!nextValue())
                UBJSON_READER_THROW("No key or value.", m_Members->tokenizer);
            [[fallthrough]];
        case ReaderState::AT_VALUE:
            switch (tokenizer.tokenType())
            {
            case UBJsonTokenType::START_OBJECT_TOKEN:
            case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
                return readObject(isExpandOptimizedByteArraysEnabled());
            case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
            case UBJsonTokenType::START_ARRAY_TOKEN:
                return readArray(isExpandOptimizedByteArraysEnabled());
            default:
                return JsonItem(UBJsonValueItem(std::string(tokenizer.token()),
                                            tokenizer.tokenType()));
            }
        case ReaderState::AT_KEY:
            return JsonItem(UBJsonValueItem(std::string(tokenizer.token()),
                                        tokenizer.tokenType()));
        default:
            UBJSON_READER_THROW("No key or value.", m_Members->tokenizer);
        }
    }

    std::string UBJsonReader::fileName() const
    {
        return m_Members->tokenizer.fileName();
    }

    size_t UBJsonReader::lineNumber() const
    {
        return 0;
    }

    size_t UBJsonReader::columnNumber() const
    {
        return m_Members->tokenizer.position();
    }

    bool UBJsonReader::isExpandOptimizedByteArraysEnabled() const
    {
        return (currentScope().state.options
                & UBJsonReaderOptions::EXPAND_OPTIMIZED_BYTE_ARRAYS) != 0;
    }

    UBJsonReader& UBJsonReader::setExpandOptimizedByteArraysEnabled(bool value)
    {
        auto& options = currentScope().state.options;
        if (value)
            options |= UBJsonReaderOptions::EXPAND_OPTIMIZED_BYTE_ARRAYS;
        else
            options &= ~UBJsonReaderOptions::EXPAND_OPTIMIZED_BYTE_ARRAYS;
        return *this;
    }

    void UBJsonReader::assertStateIsKeyOrValue() const
    {
        auto state = currentScope().state.state;
        if (state == ReaderState::AT_KEY
            || state == ReaderState::AT_VALUE)
        {
            return;
        }
        UBJSON_READER_THROW("Current token is not a key or a value.",
                            m_Members->tokenizer);
    }

    UBJsonReader::Scope& UBJsonReader::currentScope() const
    {
        if (m_Members)
            return m_Members->scopes.back();
        YSON_THROW("Uninitialized UBJsonReader.");
    }

    JsonItem UBJsonReader::readArray(bool expandOptmizedByteArrays) // NOLINT(*-no-recursion)
    {
        std::vector<JsonItem> values;
        const auto& tokenizer = m_Members->tokenizer;
        const auto arrayType = tokenizer.tokenType();
        if (arrayType == UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN
            && !expandOptmizedByteArrays)
        {
            std::string str;
            switch (tokenizer.contentType())
            {
            case UBJsonTokenType::CHAR_TOKEN:
                if (readOptimizedArrayToString<char>(*this, str))
                    return JsonItem(UBJsonValueItem(str, arrayType));
                break;
            case UBJsonTokenType::INT8_TOKEN:
                if (readOptimizedArrayToString<int8_t>(*this, str))
                    return JsonItem(UBJsonValueItem(str, arrayType));
                break;
            case UBJsonTokenType::UINT8_TOKEN:
                if (readOptimizedArrayToString<uint8_t>(*this, str))
                    return JsonItem(UBJsonValueItem(str, arrayType));
                break;
            default:
                break;
            }
        }

        enter();
        while (true)
        {
            if (!nextValue())
                break;

            switch (tokenizer.tokenType())
            {
            case UBJsonTokenType::START_OBJECT_TOKEN:
            case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
                values.push_back(readObject(expandOptmizedByteArrays));
                break;
            case UBJsonTokenType::START_ARRAY_TOKEN:
            case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
                values.push_back(readArray(expandOptmizedByteArrays));
                break;
            default:
                values.emplace_back(UBJsonValueItem(std::string(tokenizer.token()),
                                                tokenizer.tokenType()));
                break;
            }
        }
        leave();
        return JsonItem(std::make_shared<ArrayItem>(std::move(values)));
    }

    JsonItem UBJsonReader::readObject(bool expandOptimizedByteArrays) // NOLINT(*-no-recursion)
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
                UBJSON_READER_THROW("Key without value: " + keys.back(),
                                    tokenizer);
            }

            switch (tokenizer.tokenType())
            {
            case UBJsonTokenType::START_OBJECT_TOKEN:
            case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
                values.insert_or_assign(key, readObject(expandOptimizedByteArrays));
                break;
            case UBJsonTokenType::START_ARRAY_TOKEN:
            case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
                values.insert_or_assign(key, readArray(expandOptimizedByteArrays));
                break;
            default:
                values.insert_or_assign(key,
                                        JsonItem(UBJsonValueItem(std::string(tokenizer.token()),
                                                             tokenizer.tokenType())));
                break;
            }
        }
        leave();
        return JsonItem(std::make_shared<ObjectItem>(std::move(keys), std::move(values)));
    }

    template <typename T>
    bool UBJsonReader::readOptimizedArrayImpl(T* buffer, size_t& size,
                                              UBJsonTokenType tokenType)
    {
        auto& state = currentScope().state;
        auto& tokenizer = m_Members->tokenizer;
        if (state.state != ReaderState::AT_VALUE)
        {
            UBJSON_READER_THROW("Current token is not an optimized array.",
                                tokenizer);
        }

        if (tokenizer.tokenType()
            != UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN
            || tokenizer.contentType() != tokenType)
        {
            return false;
        }

        if (!buffer)
        {
            size = tokenizer.contentSize();
            return true;
        }

        if (size < tokenizer.contentSize())
            return false;

        if (tokenizer.read(buffer, size, tokenizer.contentType()))
        {
            state.state = ReaderState::AFTER_VALUE;
            return true;
        }

        return false;
    }

    ReaderState UBJsonReader::state() const
    {
        return currentScope().state.state;
    }

    std::string UBJsonReader::scope() const
    {
        std::string result;
        for (auto& scope : m_Members->scopes)
        {
            if (auto c = scope.reader->scopeType())
                result.push_back(c);

        }
        return result;
    }
}
