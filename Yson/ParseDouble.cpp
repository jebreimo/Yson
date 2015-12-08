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

namespace Yson
{

    namespace
    {
        int getDigit(char c)
        {
            return uint8_t(c) ^ 0x30u;
        }
    }
    
    std::pair<double, bool> parseDouble(const char* first, const char* last)
    {
        typedef std::pair<double, bool> Result;
        if (first == last)
            return Result(0, false);

        // Get the sign of the number
        bool negative = false;
        if (*first == '-')
        {
            negative = true;
            if (++first == last)
                return Result(0, false);
        }
        else if (*first == '+')
        {
            if (++first == last)
                return Result(0, false);
        }

        // Get the integer value
        double value = getDigit(*first);
        if (value > 9)
            return Result(0, false);
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
            auto fractStart = ++first;
            while (first != last && getDigit(*first) <= 9)
            {
                value *= 10;
                value += getDigit(*first);
                ++first;
            }
            exponent = -(int)(first - fractStart);
        }

        // Get the exponent
        if (first != last)
        {
            if ((*first & 0xDF) != 'E')
                return Result(0, false);

            if (++first == last)
                return Result(0, false);

            bool negativeExponent = false;
            if (*first == '-')
            {
                negativeExponent = true;
                if (++first == last)
                    return Result(0, false);
            }
            else if (*first == '+')
            {
                if (++first == last)
                    return Result(0, false);
            }

            int explicitExponent = getDigit(*first);
            if (explicitExponent > 9)
                return Result(0, false);

            while (++first != last)
            {
                if (getDigit(*first) > 9)
                    return Result(0, false);
                explicitExponent *= 10;
                explicitExponent += getDigit(*first);
                if (explicitExponent > 308)
                    return Result(0, false);
            }

            if (negativeExponent)
                exponent -= explicitExponent;
            else
                exponent += explicitExponent;
        }

        // Add the exponent
        if (exponent)
            value *= pow(10.0, (double)exponent);

        // Add the sign
        if (negative)
            value = -value;

        return Result(value, true);
    }

}
