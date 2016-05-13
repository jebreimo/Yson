//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseInteger.hpp"

namespace Yson {

    namespace
    {
        int64_t fromDigit(char c)
        {
            if ('0' <= c && c <= '9')
                return int64_t(c - '0');
            c &= 0xDF;
            if ('A' <= c && c <= 'Z')
                return int64_t(c - 'A');
            return INT64_MAX;
        }
    }

    std::pair<int64_t, bool> parseInteger(const char* first, const char* last,
                                          bool detectBase)
    {
        typedef std::pair<int64_t, bool> Result;
        if (first == last)
            return Result(0, false);

        int64_t sign = 1;
        if (*first == '-')
        {
            sign = -1;
            if (++first == last)
                return Result(0, false);
        }
        else if (*first == '+')
        {
            if (++first == last)
                return Result(0, false);
        }

        int64_t base = 10;
        if (*first == '0' && detectBase)
        {
            if (++first == last)
                return Result(0, false);
            switch (*first | 0x20)
            {
            case 'b':
                if (++first == last)
                    return Result(0, false);
                base = 2;
                break;
            case 'o':
                if (++first == last)
                    return Result(0, false);
                base = 8;
                break;
            case 'x':
                if (++first == last)
                    return Result(0, false);
                base = 16;
                break;
            default:
                if (*first < '0' || '9' < *first)
                    return Result(0, false);
                break;
            }
        }
        int64_t value = 0;
        while (first != last)
        {
            value *= base;
            auto digit = fromDigit(*first++);
            if (digit >= base)
                return Result(0, false);
            value += digit;
        }
        return Result(value * sign, true);
    }
}
