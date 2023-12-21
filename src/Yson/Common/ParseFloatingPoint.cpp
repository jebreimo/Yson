//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseFloatingPoint.hpp"

#include <cctype>
#include <limits>
#include <optional>
#include <fast_float.h>

namespace Yson
{
    namespace
    {
        template <typename T>
        std::optional<T> parseFloatingPoint(std::string_view str)
        {
            if (str.empty())
                return {};

            size_t i = 0;
            // Get the sign of the number
            if (str[0] == '+')
            {
                str.remove_prefix(1);
                if (str.empty())
                    return {};
            }
            else if (str[0] == '-' && ++i == str.size())
            {
                return {};
            }

            if (!isdigit(str[i]))
            {
                // Get the integer value
                if (str == "null")
                    return T();
                if (str == "Infinity" || str == "+Infinity")
                    return std::numeric_limits<T>::infinity();
                if (str == "-Infinity")
                    return -std::numeric_limits<T>::infinity();
                if (str == "NaN")
                    return std::numeric_limits<T>::quiet_NaN();
                if (str[i] != '.')
                    return {};
            }

            auto beg = str.data();
            auto end = beg + str.size();

            T value;
            auto result = fast_float::from_chars(beg, end, value);
            return result.ec == std::errc() && result.ptr == end
                   ? value
                   : std::optional<T>();
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
