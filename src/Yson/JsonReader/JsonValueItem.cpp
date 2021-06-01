//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonValueItem.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/Escape.hpp"
#include "Yson/Common/GetValueType.hpp"
#include "Yson/Common/ParseFloatingPoint.hpp"
#include "Yson/Common/ParseInteger.hpp"
#include "Yson/JsonReader/JsonTokenType.hpp"

namespace Yson
{
    JsonValueItem::JsonValueItem(std::string value, JsonTokenType type)
        : m_Value(move(value)),
          m_Type(type)
    {}

    ValueType JsonValueItem::valueType() const
    {
        return valueType(false);
    }

    ValueType JsonValueItem::valueType(bool analyzeStrings) const
    {
        if (m_Type == JsonTokenType::VALUE
            || (analyzeStrings && m_Type == JsonTokenType::STRING))
        {
            return getValueType(m_Value);
        }

        if (m_Type == JsonTokenType::STRING)
            return ValueType::STRING;

        return ValueType::INVALID;
    }

    bool JsonValueItem::isNull() const
    {
        return m_Value == "null";
    }

    bool JsonValueItem::get(bool& value) const
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

    bool JsonValueItem::get(int8_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(int16_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(int32_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(int64_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(uint8_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(uint16_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(uint32_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(uint64_t& value) const
    {
        return parse(m_Value, value, true);
    }

    bool JsonValueItem::get(float& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValueItem::get(double& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValueItem::get(long double& value) const
    {
        return parse(m_Value, value);
    }

    bool JsonValueItem::get(char& value) const
    {
        if (m_Type == JsonTokenType::VALUE)
            return parse(m_Value, value, true);
        if (m_Type != JsonTokenType::STRING)
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

    bool JsonValueItem::get(std::string& value) const
    {
        if (hasEscapedCharacters(m_Value))
            value = unescape(value);
        else
            value = m_Value;
        return true;
    }

    bool JsonValueItem::getBase64(std::vector<char>& value) const
    {
        return fromBase64(m_Value, value);
    }

    bool JsonValueItem::getBinary(std::vector<char>& value) const
    {
        return fromBase64(m_Value, value);
    }

    bool JsonValueItem::getBinary(void* buffer, size_t& size) const
    {
        return fromBase64(m_Value, static_cast<char*>(buffer), size);
    }
}
