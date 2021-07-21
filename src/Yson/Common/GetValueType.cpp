//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GetValueType.hpp"

#include "ValueTypeUtilities.hpp"

namespace Yson
{
    namespace
    {
        template<typename Predicate>
        ValueType getValueType(std::string_view str,
                               Predicate isDigit, ValueType type);

        ValueType getBinaryValueType(std::string_view str);

        ValueType getOctalValueType(std::string_view str);

        ValueType getHexadecimalValueType(std::string_view str);

        ValueType getFloatingPointValueType(std::string_view str);

        ValueType getNumberValueType(std::string_view str);
    }

    ValueType getValueType(std::string_view str)
    {
        if (str.empty())
            return ValueType::INVALID;
        auto assumedType = ValueType::UNKNOWN;

        size_t i = 0;
        if (str[i] == '-' || str[i] == '+')
        {
            if (++i == str.size())
                return ValueType::INVALID;
            assumedType = ValueType::INTEGER;
        }

        if (str[i] == '0')
        {
            if (++i == str.size())
                return ValueType::INTEGER;
            switch (str[i])
            {
            case 'b':
            case 'B':
                return getBinaryValueType(str.substr(i + 1));
            case 'o':
            case 'O':
                return getOctalValueType(str.substr(i + 1));
            case 'x':
            case 'X':
                return getHexadecimalValueType(str.substr(i + 1));
            default:
                return getNumberValueType(str.substr(i));
            }
        }
        else if (getDigit(str[i]) <= 9)
        {
            return getNumberValueType(str.substr(i));
        }

        if (str.substr(i) == "Infinity")
            return ValueType::FLOAT;
        if (str == "NaN")
            return ValueType::FLOAT;

        if (assumedType != ValueType::UNKNOWN)
            return ValueType::INVALID;

        if (str == "true")
            return ValueType::BOOLEAN;
        if (str == "null")
            return ValueType::NULL_VALUE;
        if (str == "false")
            return ValueType::BOOLEAN;

        return ValueType::INVALID;
    }

    namespace
    {
        template<typename Predicate>
        ValueType getValueType(std::string_view str,
                               Predicate isDigit, ValueType type)
        {
            if (str.empty())
                return ValueType::INVALID;
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (isDigit(str[i]))
                    continue;
                if (str[i] == '_' && 0 < i && i < str.size() - 1
                    && str[i - 1] != '_')
                    continue;
                return ValueType::INVALID;
            }
            return type;
        }

        ValueType getBinaryValueType(std::string_view str)
        {
            return getValueType(str,
                                [](char c) { return getDigit(c) <= 1; },
                                ValueType::INTEGER);
        }

        ValueType getOctalValueType(std::string_view str)
        {
            return getValueType(str,
                                [](char c) { return getDigit(c) <= 7; },
                                ValueType::INTEGER);
        }

        ValueType getHexadecimalValueType(std::string_view str)
        {
            return getValueType(str,
                                [](char c) { return isHexDigit(c); },
                                ValueType::INTEGER);
        }

        ValueType getFloatingPointValueType(std::string_view str)
        {
            if (str[0] == '.')
            {
                size_t i;
                for (i = 1; i < str.size(); ++i)
                {
                    if (getDigit(str[i]) <= 9)
                        continue;
                    if (str[i - 1] == '_')
                        return ValueType::INVALID;
                    if (str[i] == '_' && 1 < i && i < str.size() - 1)
                        continue;
                    if (str[i] == 'e' || str[i] == 'E')
                    {
                        str = str.substr(i);
                        break;
                    }
                    return ValueType::INVALID;
                }
                if (str[0] == '.')
                    return ValueType::FLOAT;
            }

            if (str[0] != 'E' && str[0] != 'e')
                return ValueType::INVALID;
            if (str.size() == 1)
                return ValueType::INVALID;

            if (str[1] != '+' && str[1] != '-')
            {
                str = str.substr(1);
            }
            else
            {
                if (str.size() == 2)
                    return ValueType::INVALID;
                str = str.substr(2);
            }
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 9)
                    continue;
                if (str[i] == '_' && 0 < i && i < str.size() - 1
                    && str[i - 1] != '_')
                    continue;
                return ValueType::INVALID;
            }
            return ValueType::FLOAT;
        }

        ValueType getNumberValueType(std::string_view str)
        {
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 9)
                    continue;

                if (str[i] == '_' && 0 < i && i < str.size() - 1
                    && str[i - 1] != '_')
                    continue;

                if ((str[i] == '.' || str[i] == 'e' || str[i] == 'E')
                    && i != 0 && str[i - 1] != '_')
                    return getFloatingPointValueType(str.substr(i));

                return ValueType::INVALID;
            }
            return ValueType::INTEGER;
        }
    }
}
