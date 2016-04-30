//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseDouble.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>

namespace Yson
{

    namespace
    {
        int getDigit(char c)
        {
            return uint8_t(c) ^ 0x30u;
        }
    }

    template <typename T>
    std::pair<T, bool> parseReal(const char* first, const char* last)
    {
        typedef std::pair<T, bool> Result;
        if (first == last)
            return Result();

        // Get the sign of the number
        bool negative = false;
        if (*first == '-')
        {
            negative = true;
            if (++first == last)
                return Result();
        }
        else if (*first == '+')
        {
            if (++first == last)
                return Result();
        }

        if (*first == 'i' && last - first == 8
            && std::equal(first, last, "infinity"))
        {
            return Result(negative ? -std::numeric_limits<T>::infinity()
                                   : std::numeric_limits<T>::infinity(),
                          true);
        }
        if (*first == 'n' && last - first == 3
            && std::equal(first, last, "nan"))
        {
            return std::make_pair(std::numeric_limits<T>::quiet_NaN(),
                                  true);
        }

        // Get the integer value
        auto value = T(getDigit(*first));
        if (value > 9)
            return Result();
        while (true)
        {
            if (++first == last)
            {
                if (negative)
                    value = -value;
                return Result(value, true);
            }
            auto digit = getDigit(*first);
            if (digit > 9)
                break;
            value *= 10;
            value += digit;
        }

        // Get the fraction
        int exponent = 0;
        if (*first == '.')
        {
            auto fractionStart = ++first;
            while (first != last && getDigit(*first) <= 9)
            {
                value *= 10;
                value += getDigit(*first);
                ++first;
            }
            exponent = -(int)(first - fractionStart);
        }

        // Get the exponent
        if (first != last)
        {
            if ((*first & 0xDF) != 'E')
                return Result();

            if (++first == last)
                return Result();

            bool negativeExponent = false;
            if (*first == '-')
            {
                negativeExponent = true;
                if (++first == last)
                    return Result();
            }
            else if (*first == '+')
            {
                if (++first == last)
                    return Result();
            }

            int explicitExponent = getDigit(*first);
            if (explicitExponent > 9)
                return Result();

            while (++first != last)
            {
                if (getDigit(*first) > 9)
                    return Result();
                explicitExponent *= 10;
                explicitExponent += getDigit(*first);
                if (explicitExponent > std::numeric_limits<T>::max_exponent10)
                    return Result();
            }

            if (negativeExponent)
                exponent -= explicitExponent;
            else
                exponent += explicitExponent;
        }

        // Add the exponent
        if (exponent)
            value *= pow(T(10), exponent);

        // Add the sign
        if (negative)
            value = -value;

        return Result(value, true);
    }

    std::pair<double, bool> parseDouble(const char* first, const char* last)
    {
        return parseReal<double>(first, last);
    }

    std::pair<float, bool> parseFloat(const char* first, const char* last)
    {
        return parseReal<float>(first, last);
    }
}
