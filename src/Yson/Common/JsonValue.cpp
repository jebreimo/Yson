//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonValue.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/Escape.hpp"
#include "Yson/Common/GetValueType.hpp"
#include "Yson/Common/ParseFloatingPoint.hpp"
#include "Yson/Common/ParseInteger.hpp"
#include "Yson/JsonReader/JsonTokenType.hpp"

namespace Yson
{
    JsonValue::JsonValue(std::string value, int type)
        : m_Value(move(value)),
          m_Type(type)
    {}

    ValueType JsonValue::valueType(bool analyzeStrings) const
    {
        auto tokenType = JsonTokenType(m_Type);
        if (tokenType == JsonTokenType::VALUE
            || (analyzeStrings && tokenType == JsonTokenType::STRING))
        {
            return getValueType(m_Value);
        }

        if (tokenType == JsonTokenType::STRING)
            return ValueType::STRING;

        return ValueType::INVALID;
    }

    bool JsonValue::isNull() const
    {
        return m_Value == "null";
    }

    bool JsonValue::get(bool& value) const
    {
        if (m_Value == "true" || m_Value == "1")
        {
            value = true;
            return true;
        }

        if (m_Value == "false" || m_Value == "0" || m_Value == "null")
        {
            value = false;
            return true;
        }

        return false;
    }

    bool JsonValue::get(int8_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(int16_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(int32_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(int64_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(uint8_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(uint16_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(uint32_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(uint64_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValue::get(float& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValue::get(double& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValue::get(long double& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValue::get(char& value) const
    {
        if (JsonTokenType(m_Type) == JsonTokenType::VALUE)
            return parse(m_Value, value, true);
        if (JsonTokenType(m_Type) != JsonTokenType::STRING)
            return false;
        if (m_Value.size() == 1)
        {
            value = m_Value[0];
            return true;
        }
        if (hasEscapedCharacters(m_Value))
        {
            auto str = unescape(m_Value);
            if (str.size() == 1)
            {
                value = str[0];
                return true;
            }
        }
        return false;
    }

    bool JsonValue::get(std::string& value) const
    {
        if (hasEscapedCharacters(m_Value))
            value = unescape(value);
        else
            value = m_Value;
        return true;
    }

    bool JsonValue::getBase64(std::vector<char>& value) const
    {
        return fromBase64(m_Value, value);
    }

    bool JsonValue::getBinary(std::vector<char>& value) const
    {
        return fromBase64(m_Value, value);
    }

    bool JsonValue::getBinary(void* buffer, size_t& size) const
    {
        return fromBase64(m_Value, static_cast<char*>(buffer), size);
    }
}
