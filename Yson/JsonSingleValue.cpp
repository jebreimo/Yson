//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonSingleValue.hpp"

#include "GetValueType.hpp"
#include "ParseDouble.hpp"
#include "ParseInteger.hpp"
#include "YsonException.hpp"
#include "Base64.hpp"

namespace Yson
{
    namespace
    {
        const std::string StringTypeName = "String";
        const std::string ValueTypeName = "Value";
        const std::string UnknownTypeName = "Unknown";
    }

    JsonSingleValue::JsonSingleValue(Yson::StringRef stringRef,
                                     const std::shared_ptr<std::string>& buffer,
                                     TokenType tokenType)
            : m_StringRef(stringRef),
              m_Buffer(buffer),
              m_TokenType(tokenType)
    {}

    ValueType JsonSingleValue::valueType() const
    {
        return getValueType(m_StringRef.begin(), m_StringRef.end());
    }

    const std::string& JsonSingleValue::typeName() const
    {
        if (m_TokenType == TokenType::STRING
            || m_TokenType == TokenType::BLOCK_STRING)
            return StringTypeName;
        else if (m_TokenType == TokenType::VALUE)
            return ValueTypeName;
        else
            return UnknownTypeName;
    }

    bool JsonSingleValue::value(std::string& value) const
    {
        value = std::string(m_StringRef.string, m_StringRef.size);
        return true;
    }

    bool JsonSingleValue::value(bool& value) const
    {
        auto str = std::string(m_StringRef.string, m_StringRef.size);
        if (str == "true")
            value = true;
        else if (str == "false")
            value = false;
        else
            return false;
        return true;
    }

    bool JsonSingleValue::value(int8_t& value) const
    {
        return parseSignedInteger(value);
    }

    bool JsonSingleValue::value(int16_t& value) const
    {
        return parseSignedInteger(value);
    }

    bool JsonSingleValue::value(int32_t& value) const
    {
        return parseSignedInteger(value);
    }

    bool JsonSingleValue::value(int64_t& value) const
    {
        return parseSignedInteger(value);
    }

    bool JsonSingleValue::value(uint8_t& value) const
    {
        return parseUnsignedInteger(value);
    }

    bool JsonSingleValue::value(uint16_t& value) const
    {
        return parseUnsignedInteger(value);
    }

    bool JsonSingleValue::value(uint32_t& value) const
    {
        return parseUnsignedInteger(value);
    }

    bool JsonSingleValue::value(uint64_t& value) const
    {
        return parseUnsignedInteger(value);
    }

    bool JsonSingleValue::value(float& value) const
    {
        auto parsedValue = parseFloat(m_StringRef.begin(),
                                      m_StringRef.end());
        if (parsedValue.second)
            value = parsedValue.first;
        return parsedValue.second;
    }

    bool JsonSingleValue::value(double& value) const
    {
        auto parsedValue = parseDouble(m_StringRef.begin(),
                                       m_StringRef.end());
        if (parsedValue.second)
            value = parsedValue.first;
        return parsedValue.second;
    }

    bool JsonSingleValue::value(long double& value) const
    {
        auto parsedValue = parseLongDouble(m_StringRef.begin(),
                                           m_StringRef.end());
        if (parsedValue.second)
            value = parsedValue.first;
        return parsedValue.second;
    }

    bool JsonSingleValue::value(std::vector<uint8_t>& value) const
    {
        try
        {
            auto str = std::string(m_StringRef.string, m_StringRef.size);
            value = fromBase64(str);
            return true;
        }
        catch (std::exception& ex)
        {
            return false;
        }
    }

    size_t JsonSingleValue::updateStringRef(size_t offset)
    {
        m_StringRef.string = m_Buffer->data() + offset;
        return offset + m_StringRef.size;
    }

    template <typename T>
    bool JsonSingleValue::parseSignedInteger(T& value) const
    {
        auto parsedValue = parseInteger(m_StringRef.begin(),
                                        m_StringRef.end(),
                                        true);
        if (!parsedValue.second)
            return false;
        auto actualValue = static_cast<T>(parsedValue.first);
        if (actualValue == parsedValue.first)
        {
            value = actualValue;
            return true;
        }
        return false;
    }

    template <typename T>
    bool JsonSingleValue::parseUnsignedInteger(T& value) const
    {
        if (m_StringRef.size == 0 || *m_StringRef.begin() == '-')
            return false;

        auto parsedValue = parseInteger(m_StringRef.begin(),
                                        m_StringRef.end(),
                                        true);
        if (!parsedValue.second)
            false;
        auto unsignedValue = static_cast<uint64_t>(parsedValue.first);
        auto actualValue = static_cast<T>(unsignedValue);
        if (actualValue == unsignedValue)
        {
            value = actualValue;
            return true;
        }
        return false;
    }
}
