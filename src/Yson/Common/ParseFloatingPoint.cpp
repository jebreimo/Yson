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
#include <optional>

namespace Yson
{
    namespace
    {
        int getDigit(char c)
        {
            return int(uint8_t(c) ^ 0x30u);
        }

        template <typename T>
        std::optional<T> parseFloatingPoint(std::string_view str)
        {
            if (str.empty())
                return {};

            size_t i = 0;
            // Get the sign of the number
            bool negative = false;
            if (str[0] == '-')
            {
                negative = true;
                if (++i == str.size())
                    return {};
            }
            else if (str[0] == '+')
            {
                if (++i == str.size())
                    return {};
            }

            // Get the integer value
            auto value = T(getDigit(str[i]));
            if (value > 9)
            {
                if (str == "Infinity" || str == "null" || str == "+Infinity")
                    return std::numeric_limits<T>::infinity();
                if (str == "-Infinity")
                    return -std::numeric_limits<T>::infinity();
                if (str == "NaN")
                    return std::numeric_limits<T>::quiet_NaN();
                return {};
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
                return {};

            if (i == str.size())
                return !negative ? value : -value;

            // Get the fraction
            underscore = true; // Makes underscore after point illegal.
            int decimals = 0;
            T fraction = {};
            if (str[i] == '.')
            {
                for (++i; i < str.size(); ++i)
                {
                    auto digit = getDigit(str[i]);
                    if (digit <= 9)
                    {
                        fraction *= 10;
                        fraction += digit;
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
                if ((uint8_t(str[i]) & 0xDFu) != 'E')
                    return {};

                if (++i == str.size())
                    return {};

                bool negativeExponent = false;
                if (str[i] == '-')
                {
                    negativeExponent = true;
                    if (++i == str.size())
                        return {};
                }
                else if (str[i] == '+')
                {
                    if (++i == str.size())
                        return {};
                }

                exponent += getDigit(str[i]);
                if (exponent > 9)
                    return {};

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
                        return {};
                    }
                    else
                    {
                        underscore = true;
                    }
                    if (exponent > std::numeric_limits<T>::max_exponent10)
                        return {};
                }
                if (negativeExponent)
                    exponent = -exponent;
            }

            if (exponent)
                value *= std::pow(T(10), T(exponent));
            if (fraction != 0)
                value += fraction * std::pow(T(10), T(exponent - decimals));

            // Add the sign
            if (negative)
                value = -value;

            return value;
        }

        template <typename T>
        bool parseImpl(std::string_view str, T& value)
        {
            if (auto parsedValue = parseFloatingPoint<T>(str))
            {
                value = *parsedValue;
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
