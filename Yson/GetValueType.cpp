//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GetValueType.hpp"

#include <cstdint>

namespace Yson {

    namespace {
        int getDigit(char c);

        bool isHexDigit(char c);

        template<typename Predicate>
        ValueType_t getValueType(const char* first, const char* last,
                                 Predicate isDigit, ValueType_t type);

        ValueType_t getBinaryValueType(const char* first, const char* last);

        ValueType_t getOctalValueType(const char* first, const char* last);

        ValueType_t getHexadecimalValueType(const char* first,
                                            const char* last);

        ValueType_t getFloatingPointValueType(const char* first,
                                              const char* last);

        ValueType_t getNumberValueType(const char* first, const char* last);
    }

    ValueType_t getValueType(const char* first, const char* last)
    {
        if (first == last)
            return ValueType::INVALID;
        auto assumedType = ValueType_t(ValueType::UNKNOWN);
        if (*first == '-' || *first == '+')
        {
            if (++first == last)
                return ValueType::INVALID;
            assumedType = ValueType::INTEGER;
        }
        if (*first == '0')
        {
            if (++first == last)
                return ValueType::INTEGER;
            switch (*first)
            {
            case 'b':
            case 'B':
                return getBinaryValueType(++first, last);
            case 'o':
            case 'O':
                return getOctalValueType(++first, last);
            case 'x':
            case 'X':
                return getHexadecimalValueType(++first, last);
            case '.':
            case 'e':
            case 'E':
                return getFloatingPointValueType(first, last);
            default:
                if (getDigit(*first) <= 9)
                {
                    if (++first == last)
                        return ValueType::INTEGER;
                    return getNumberValueType(first, last);
                }
                return ValueType::INVALID;
            }
        }
        else if (getDigit(*first) <= 9)
        {
            if (++first == last)
                return ValueType::INTEGER;
            return getNumberValueType(first, last);
        }

        if (last - first == 8 && std::equal(first, last, "infinity"))
            return ValueType::EXTENDED_FLOAT;
        if (last - first == 3 && std::equal(first, last, "NaN"))
            return ValueType::EXTENDED_FLOAT;

        if (assumedType != ValueType::UNKNOWN)
            return ValueType::INVALID;

        if (last - first == 4 && std::equal(first, last, "true"))
            return ValueType::TRUE_VALUE;
        if (last - first == 4 && std::equal(first, last, "null"))
            return ValueType::NULL_VALUE;
        if (last - first == 5 && std::equal(first, last, "false"))
            return ValueType::FALSE_VALUE;

        return ValueType::INVALID;
    }

    ValueType_t getValueType(const std::pair<const char*, const char*>& value)
    {
        return getValueType(value.first, value.second);
    }

    ValueType_t getValueType(const std::string& s)
    {
        return getValueType(s.data(), s.data() + s.size());
    }

    namespace {

        int getDigit(char c)
        {
            return uint8_t(c) ^ 0x30;
        }

        bool isHexDigit(char c)
        {
            return (getDigit(c) <= 9) ||
                   ('A' <= c && c <= 'F') ||
                   ('a' <= c && c <= 'f');
        }

        template<typename Predicate>
        ValueType_t getValueType(const char* first, const char* last,
                                 Predicate isDigit, ValueType_t type)
        {
            if (first == last)
                return ValueType::INVALID;
            while (isDigit(*first))
            {
                if (++first == last)
                    return type;
            }
            return ValueType::INVALID;
        }

        ValueType_t getBinaryValueType(const char* first, const char* last)
        {
            return getValueType(first, last,
                                [](char c) { return getDigit(c) <= 1; },
                                ValueType::BIN_INTEGER);
        }

        ValueType_t getOctalValueType(const char* first, const char* last)
        {
            return getValueType(first, last,
                                [](char c) { return getDigit(c) <= 7; },
                                ValueType::OCT_INTEGER);
        }

        ValueType_t getHexadecimalValueType(const char* first,
                                            const char* last)
        {
            return getValueType(first, last,
                                [](char c) { return isHexDigit(c); },
                                ValueType::HEX_INTEGER);
        }

        ValueType_t getFloatingPointValueType(const char* first,
                                              const char* last)
        {
            if (*first == '.')
            {
                if (++first == last)
                    return ValueType::FLOAT;
                while (getDigit(*first) <= 9)
                {
                    if (++first == last)
                        return ValueType::FLOAT;
                }
                if (*first != 'E' && *first != 'e')
                    return ValueType::INVALID;
            }

            ++first;
            if (*first == '+' || *first == '-')
            {
                if (++first == last)
                    return ValueType::INVALID;
            }
            while (getDigit(*first) <= 9)
            {
                if (++first == last)
                    return ValueType::FLOAT;
            }
            return ValueType::INVALID;
        }

        ValueType_t getNumberValueType(const char* first, const char* last)
        {
            while (getDigit(*first) <= 9)
            {
                if (++first == last)
                    return ValueType::INTEGER;
            }

            switch (*first)
            {
            case '.':
            case 'e':
            case 'E':
                return getFloatingPointValueType(first, last);
            default:
                return ValueType::INVALID;
            }
        }
    }
}
