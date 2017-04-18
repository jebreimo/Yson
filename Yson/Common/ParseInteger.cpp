//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 23.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseInteger.hpp"
#include <limits>
#include <type_traits>
#include <utility>
#include "SelectTypeIf.hpp"

namespace Yson
{
    namespace
    {
        template <typename IntT>
        IntT fromDigit(char c)
        {
            if ('0' <= c && c <= '9')
                return IntT(c - '0');
            c &= 0xDF;
            if ('A' <= c && c <= 'Z')
                return IntT(10 + c - 'A');
            return std::numeric_limits<IntT>::max();
        }

        template <typename T, T Base, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
        constexpr T maxPrecedingDigitsNegative()
        {
            using U = typename std::make_unsigned<T>::type;
            return T((U(1) << (sizeof(T) * 8 - 1)) / U(Base));
        }

        template <typename T, T Base, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
        constexpr T maxPrecedingDigitsNegative()
        {
            return T(0);
        }

        template <typename T, T Base, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
        constexpr T maxFinalDigitNegative()
        {
            using U = typename std::make_unsigned<T>::type;
            return T((U(1) << (sizeof(T) * 8 - 1)) % U(Base));
        }

        template <typename T, T Base, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
        constexpr T maxFinalDigitNegative()
        {
            return T(0);
        }

        template <typename T, T Base, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
        constexpr T maxPrecedingDigitsPositive()
        {
            using U = typename std::make_unsigned<T>::type;
            return T(U(~(U(1) << (sizeof(T) * 8 - 1))) / U(Base));
        }

        template <typename T, T Base, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
        constexpr T maxPrecedingDigitsPositive()
        {
            return T(~T(0)) / Base;
        }

        template <typename T, T Base, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
        constexpr T maxFinalDigitPositive()
        {
            using U = typename std::make_unsigned<T>::type;
            return T(U(~(U(1) << (sizeof(T) * 8 - 1))) % U(Base));
        }

        template <typename T, T Base, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
        constexpr T maxFinalDigitPositive()
        {
            return T(~T(0)) % Base;
        }

        template <typename IntT, IntT Base>
        std::pair<IntT, bool> parsePositiveIntegerImpl(std::string_view str)
        {
            if (str.empty())
                return std::make_pair(0, false);
            IntT value = fromDigit<IntT>(str[0]);
            if (value >= Base)
                return std::make_pair(0, false);
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = fromDigit<IntT>(str[i]);
                if (digit < Base
                    && (value < maxPrecedingDigitsPositive<IntT, Base>()
                        || (value == maxPrecedingDigitsPositive<IntT, Base>()
                            && digit <= maxFinalDigitPositive<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return std::make_pair(0, false);
                }
            }
            return std::make_pair(value, true);
        }

        struct IsSigned {};

        struct IsUnsigned {};

        template <typename IntT, IntT Base>
        std::pair<IntT, bool> parseNegativeIntegerImpl(std::string_view str,
                                                       IsSigned)
        {
            if (str.empty())
                return std::make_pair(0, false);
            IntT value = fromDigit<IntT>(str[0]);
            if (value >= Base)
                return std::make_pair(0, false);
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = fromDigit<IntT>(str[i]);
                if (digit < Base
                    && (value < maxPrecedingDigitsNegative<IntT, Base>()
                        || (value == maxPrecedingDigitsNegative<IntT, Base>()
                            && digit <= maxFinalDigitNegative<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return std::make_pair(0, false);
                }
            }
            return std::make_pair(-value, true);
        }

        template <typename IntT, IntT Base>
        std::pair<IntT, bool> parseNegativeIntegerImpl(std::string_view str,
                                                       IsUnsigned)
        {
            if (str.empty())
                return std::make_pair(0, false);
            for (size_t i = 0; i < str.size(); ++i)
            {
                auto digit = fromDigit<IntT>(str[i]);
                if (digit > 0)
                    return std::make_pair(0, false);
            }
            return std::make_pair(0, true);
        }

        template <typename IntT>
        std::pair<IntT, bool>
        parseInteger(std::string_view str, bool detectBase)
        {
            using Signedness = typename SelectTypeIf<
                    std::is_signed<IntT>::value,
                    IsSigned,
                    IsUnsigned>::type;
            using Result = std::pair<IntT, bool>;
            if (str.empty())
                return Result(0, false);

            bool positive = true;
            size_t i = 0;
            if (str[0] == '-')
            {
                positive = false;
                if (str.size() == 1)
                    return Result(0, false);
                i = 1;
            }
            else if (str[0] == '+')
            {
                if (str.size() == 1)
                    return Result(0, false);
                i = 1;
            }

            int64_t base = 10;
            if (str[i] == '0' && detectBase)
            {
                if (++i == str.size())
                    return Result(0, true);
                auto str2 = str.substr(i + 1);
                switch (str[i] | 0x20)
                {
                case 'b':
                    return positive ? parsePositiveIntegerImpl<IntT, 2>(str2)
                                    : parseNegativeIntegerImpl<IntT, 2>(
                                              str2, Signedness());
                case 'o':
                    return positive ? parsePositiveIntegerImpl<IntT, 8>(str2)
                                    : parseNegativeIntegerImpl<IntT, 8>(
                                              str2, Signedness());
                case 'x':
                    return positive ? parsePositiveIntegerImpl<IntT, 16>(str2)
                                    : parseNegativeIntegerImpl<IntT, 16>(
                                              str2, Signedness());
                case '_':
                    if (str2.empty())
                        return Result(0, false);
                    //[[fallthrough]]
                default:
                    return positive ? parsePositiveIntegerImpl<IntT, 10>(str2)
                                    : parseNegativeIntegerImpl<IntT, 10>(
                                              str2, Signedness());
                }
            }
            if ('0' <= str[i] && str[i] <= '9')
            {
                auto str2 = str.substr(i);
                return positive ? parsePositiveIntegerImpl<IntT, 10>(str2)
                                : parseNegativeIntegerImpl<IntT, 10>(
                                          str2, Signedness());
            }
            if (str == "false")
                return Result(0, true);
            if (str == "null")
                return Result(0, true);
            if (str == "true")
                return Result(1, true);
            return Result(0, true);
        }

        template <typename T>
        bool parseImpl(std::string_view str, T& value, bool detectBase)
        {
            auto parsedValue = parseInteger<T>(str, detectBase);
            if (parsedValue.second)
            {
                value = parsedValue.first;
                return true;
            }
            return false;
        }
    }

    bool parse(std::string_view str, char& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, int8_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, int16_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, int32_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, int64_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, uint8_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, uint16_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, uint32_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }

    bool parse(std::string_view str, uint64_t& value, bool detectBase)
    {
        return parseImpl(str, value, detectBase);
    }
}
