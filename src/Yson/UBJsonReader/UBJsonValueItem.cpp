//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/UBJsonValueItem.hpp"

#include "Yson/YsonException.hpp"
#include "Yson/Common/AssignInteger.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/GetValueType.hpp"
#include "Yson/Common/ParseFloatingPoint.hpp"
#include "Yson/Common/ParseInteger.hpp"
#include "FromBigEndian.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    namespace
    {
        template <typename T>
        T getNativeValue(std::string_view data)
        {
            assert(data.size() == sizeof(T));
            T tmp = *reinterpret_cast<const T*>(data.data());
            T result;
            fromBigEndian<sizeof(T)>(reinterpret_cast<char*>(&result),
                                     reinterpret_cast<const char*>(&tmp));
            return result;
        }

        template <typename T>
        bool setIntegerValue(T& value,
                             UBJsonTokenType type,
                             std::string_view data)
        {
            switch (type)
            {
            case UBJsonTokenType::NULL_TOKEN:
                value = 0;
                return true;
            case UBJsonTokenType::TRUE_TOKEN:
                value = 1;
                return true;
            case UBJsonTokenType::FALSE_TOKEN:
                value = 0;
                return true;
            case UBJsonTokenType::CHAR_TOKEN:
            case UBJsonTokenType::INT8_TOKEN:
                return assignInteger(value, getNativeValue<int8_t>(data));
            case UBJsonTokenType::UINT8_TOKEN:
                return assignInteger(value, getNativeValue<uint8_t>(data));
            case UBJsonTokenType::INT16_TOKEN:
                return assignInteger(value, getNativeValue<int16_t>(data));
            case UBJsonTokenType::INT32_TOKEN:
                return assignInteger(value, getNativeValue<int32_t>(data));
            case UBJsonTokenType::INT64_TOKEN:
                return assignInteger(value, getNativeValue<int64_t>(data));
            case UBJsonTokenType::STRING_TOKEN:
                return parse(data, value, true);
            default:
                YSON_THROW("Not an integer value." + toString(type));
            }
        }

        template <typename T>
        bool setFloatingPointValue(T& value,
                                   UBJsonTokenType type,
                                   std::string_view data)
        {
            switch (type)
            {
            case UBJsonTokenType::NULL_TOKEN:
                value = std::numeric_limits<T>::infinity();
                return true;
            case UBJsonTokenType::TRUE_TOKEN:
                value = 1;
                return true;
            case UBJsonTokenType::FALSE_TOKEN:
                value = 0;
                return true;
            case UBJsonTokenType::CHAR_TOKEN:
            case UBJsonTokenType::INT8_TOKEN:
                value = T(getNativeValue<int8_t>(data));
                return true;
            case UBJsonTokenType::UINT8_TOKEN:
                value = T(getNativeValue<uint8_t>(data));
                return true;
            case UBJsonTokenType::INT16_TOKEN:
                value = T(getNativeValue<int16_t>(data));
                return true;
            case UBJsonTokenType::INT32_TOKEN:
                value = T(getNativeValue<int32_t>(data));
                return true;
            case UBJsonTokenType::INT64_TOKEN:
                value = T(getNativeValue<int64_t>(data));
                return true;
            case UBJsonTokenType::FLOAT32_TOKEN:
                value = T(getNativeValue<float>(data));
                return true;
            case UBJsonTokenType::FLOAT64_TOKEN:
                value = T(getNativeValue<double>(data));
                return true;
            case UBJsonTokenType::STRING_TOKEN:
            case UBJsonTokenType::HIGH_PRECISION_TOKEN:
                return parse(data, value);
            default:
                YSON_THROW("Not a floating point value: " + toString(type));
            }
        }
    }

    UBJsonValueItem::UBJsonValueItem(std::string value, UBJsonTokenType type)
        : m_Value(std::move(value)),
          m_Type(type)
    {}

    ValueType UBJsonValueItem::valueType() const
    {
        return valueType(false);
    }

    ValueType UBJsonValueItem::valueType(bool analyzeStrings) const
    {
        switch (m_Type)
        {
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
                auto type = getValueType(m_Value);
                if (type != ValueType::INVALID)
                    return type;
            }
            return ValueType::STRING;
        default:
            return ValueType::INVALID;
        }
    }

    bool UBJsonValueItem::isNull() const
    {
        return m_Type == UBJsonTokenType::NULL_TOKEN;
    }

    bool UBJsonValueItem::get(bool& value) const
    {
        switch (m_Type)
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

    bool UBJsonValueItem::get(int8_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(int16_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(int32_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(int64_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(uint8_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(uint16_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(uint32_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(uint64_t& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(float& value) const
    {
        return setFloatingPointValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(double& value) const
    {
        return setFloatingPointValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(long double& value) const
    {
        return setFloatingPointValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(char& value) const
    {
        return setIntegerValue(value, m_Type, m_Value);
    }

    bool UBJsonValueItem::get(std::string& value) const
    {
        switch (m_Type)
        {
        case UBJsonTokenType::STRING_TOKEN:
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
        case UBJsonTokenType::CHAR_TOKEN:
        {
            value = m_Value;
            return true;
        }
        default:
            return false;
        }
    }

    bool UBJsonValueItem::getBase64(std::vector<char>& value) const
    {
        if (m_Type == UBJsonTokenType::STRING_TOKEN)
            return fromBase64(m_Value, value);
        return false;
    }

    bool UBJsonValueItem::getBinary(std::vector<char>& value) const
    {
        size_t size;
        if (getBinary(nullptr, size))
        {
            value.resize(size);
            return getBinary(value.data(), size);
        }
        return false;
    }

    bool UBJsonValueItem::getBinary(void* buffer, size_t& size) const
    {
        if (m_Type != UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN
            && m_Type != UBJsonTokenType::STRING_TOKEN)
        {
            YSON_THROW("Value is not suitable for getBinary value: "
                       + toString(m_Type));
        }

        if (buffer == nullptr)
        {
            size = m_Value.size();
            return true;
        }

        if (size < m_Value.size())
            return false;

        std::copy(m_Value.begin(), m_Value.end(), static_cast<char*>(buffer));
        size = m_Value.size();
        return true;
    }
}
