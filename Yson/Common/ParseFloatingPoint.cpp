//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseFloatingPoint.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

namespace Yson
{
    namespace
    {
        int getDigit(char c)
        {
            return uint8_t(c) ^ 0x30u;
        }

        template <typename T>
        std::pair<T, bool> parseFloatingPoint(std::string_view str)
        {
            typedef std::pair<T, bool> Result;
            constexpr Result BadResult(T(0), false);
            if (str.empty())
                return BadResult;

            size_t i = 0;
            // Get the sign of the number
            bool negative = false;
            if (str[0] == '-')
            {
                negative = true;
                if (++i == str.size())
                    return BadResult;
            }
            else if (str[0] == '+')
            {
                if (++i == str.size())
                    return BadResult;
            }

            // Get the integer value
            auto value = T(getDigit(str[i]));
            if (value > 9)
            {
                if (str == "infinity" || str == "null" || str == "+infinity")
                    return Result(std::numeric_limits<T>::infinity(), true);
                if (str == "-infinity")
                    return Result(-std::numeric_limits<T>::infinity(), true);
                if (str == "NaN")
                    return Result(std::numeric_limits<T>::quiet_NaN(), true);
                return BadResult;
            }

            bool underscore = false;
            for (++i; i < str.size(); ++i)
            {
                auto digit = getDigit(str[i]);
                if (digit <= 9)
                {
                    value *= 10;
                    value += digit;
                    underscore = false;
                }
                else if (str[i] != '_' || underscore)
                {
                    break;
                }
                else
                {
                    underscore = true;
                }
            }

            if (underscore)
                return BadResult;

            if (i == str.size())
                return Result(!negative ? value : -value, true);

            // Get the fraction
            int decimals = 0;
            underscore = true; // Makes underscore after point illegal.
            if (str[i] == '.')
            {
                for (++i; i < str.size(); ++i)
                {
                    auto digit = getDigit(str[i]);
                    if (digit <= 9)
                    {
                        value *= 10;
                        value += digit;
                        underscore = false;
                        ++decimals;
                    }
                    else if (str[i] != '_' || underscore)
                    {
                        break;
                    }
                    else
                    {
                        underscore = true;
                    }
                }
            }

            // Get the exponent
            int exponent = 0;
            if (i != str.size())
            {
                if ((str[i] & 0xDF) != 'E')
                    return BadResult;

                if (++i == str.size())
                    return BadResult;

                bool negativeExponent = false;
                if (str[i] == '-')
                {
                    negativeExponent = true;
                    if (++i == str.size())
                        return BadResult;
                }
                else if (str[i] == '+')
                {
                    if (++i == str.size())
                        return BadResult;
                }

                exponent += getDigit(str[i]);
                if (exponent > 9)
                    return BadResult;

                for (++i; i != str.size(); ++i)
                {
                    auto digit = getDigit(str[i]);
                    if (digit <= 9)
                    {
                        exponent *= 10;
                        exponent += digit;
                        underscore = false;
                    }
                    else if (str[i] != '_' || underscore)
                    {
                        return BadResult;
                    }
                    else
                    {
                        underscore = true;
                    }
                    if (exponent > std::numeric_limits<T>::max_exponent10)
                        return BadResult;
                }
                if (negativeExponent)
                    exponent = -exponent;
            }

            exponent -= decimals;
            if (exponent >= std::numeric_limits<T>::min_exponent10)
            {
              // Add the exponent
              if (exponent)
                  value *= pow(T(10), exponent);
            }
            else
            {
                // The value is awfully close to 0. Try to restore it in
                // two steps.
                value /= pow(T(10), decimals);
                value *= pow(T(10), exponent + decimals);
            }

            // Add the sign
            if (negative)
                value = -value;

            return Result(value, true);
        }

        template <typename T>
        bool parseImpl(std::string_view str, T& value)
        {
            auto parsedValue = parseFloatingPoint<T>(str);
            if (parsedValue.second)
            {
                value = parsedValue.first;
                return true;
            }
            return false;
        }
    }

    bool parse(std::string_view str, float& value)
    {
        return parseImpl(str, value);
    }

    bool parse(std::string_view str, double& value)
    {
        return parseImpl(str, value);
    }

    bool parse(std::string_view str, long double& value)
    {
        return parseImpl(str, value);
    }
}
