//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GetDetailedValueType.hpp"

#include <tuple>
#include <vector>
#include "ValueTypeUtilities.hpp"

namespace Yson
{
    namespace
    {
        DetailedValueType getBinaryNumberType(std::string_view str,
                                              bool negative);

        DetailedValueType getOctalNumberType(std::string_view str,
                                             bool negative);

        DetailedValueType getHexadecimalNumberType(std::string_view str,
                                                   bool negative);

        DetailedValueType getFloatingPointNumberType(std::string_view str);

        DetailedValueType getNumberType(std::string_view str, bool negative);
    }

    DetailedValueType getDetailedValueType(std::string_view str)
    {
        if (str.empty())
            return DetailedValueType::INVALID;
        auto assumedType = DetailedValueType::UNKNOWN;
        size_t i = 0;
        bool negative = false;
        if (str[i] == '-' || str[i] == '+')
        {
            negative = str[i] == '-';
            if (++i == str.size())
                return DetailedValueType::INVALID;
            assumedType = DetailedValueType::BIG_INT;
        }
        if (str[i] == '0')
        {
            if (++i == str.size())
                return DetailedValueType::UINT_7;
            switch (str[i])
            {
            case 'b':
            case 'B':
                return getBinaryNumberType(str.substr(i + 1), negative);
            case 'o':
            case 'O':
                return getOctalNumberType(str.substr(i + 1), negative);
            case 'x':
            case 'X':
                return getHexadecimalNumberType(str.substr(i + 1), negative);
            case '.':
            case 'e':
            case 'E':
                return getFloatingPointNumberType(str.substr(i));
            default:
                return getNumberType(str.substr(i), negative);
            }
        }
        else if (getDigit(str[i]) <= 9)
        {
            return getNumberType(str.substr(i), negative);
        }

        if (str.substr(i) == "Infinity")
            return DetailedValueType::FLOAT_64;
        if (str == "NaN")
            return DetailedValueType::FLOAT_64;

        if (assumedType != DetailedValueType::UNKNOWN)
            return DetailedValueType::INVALID;

        if (str == "true")
            return DetailedValueType::BOOLEAN;
        if (str == "null")
            return DetailedValueType::NULL_VALUE;
        if (str == "false")
            return DetailedValueType::BOOLEAN;

        return DetailedValueType::INVALID;
    }

    namespace
    {
        DetailedValueType getFloatingPointNumberType(std::string_view str)
        {
            if (str[0] == '.')
            {
                size_t i;
                for (i = 1; i < str.size(); ++i)
                {
                    if (getDigit(str[i]) <= 9)
                        continue;
                    if (str[i - 1] == '_')
                        return DetailedValueType::INVALID;
                    if (str[i] == '_' && 1 < i && i < str.size() - 1)
                        continue;
                    if (str[i] == 'e' || str[i] == 'E')
                    {
                        str = str.substr(i);
                        break;
                    }
                    return DetailedValueType::INVALID;
                }
                if (str[0] == '.')
                    return DetailedValueType::FLOAT_64;
            }
            if (str[0] != 'E' && str[0] != 'e')
                return DetailedValueType::INVALID;
            if (str.size() == 1)
                return DetailedValueType::INVALID;

            if (str[1] != '+' && str[1] != '-')
            {
                str = str.substr(1);
            }
            else
            {
                if (str.size() == 2)
                    return DetailedValueType::INVALID;
                str = str.substr(2);
            }
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 9)
                    continue;
                if (str[i] == '_' && 0 < i && i < str.size() - 1
                    && str[i - 1] != '_')
                    continue;
                return DetailedValueType::INVALID;
            }
            return DetailedValueType::FLOAT_64;
        }

        bool isAllZeros(std::string_view str)
        {
            for (auto c : str)
            {
                if (c != '0' && c != '_')
                    return false;
            }
            return true;
        }

        bool isLessThanOrEqualNumber(std::string_view str,
                                     std::string_view cmp)
        {
            size_t i = 0, j = 0;
            while (i < str.size() && j < cmp.size())
            {
                if (str[i] != cmp[j])
                {
                    if (str[i] != '_')
                        return str[i] < cmp[j];
                    ++i;
                }
                else
                {
                    ++i;
                    ++j;
                }
            }
            while (i < str.size() && str[i] == '_')
                ++i;
            return i == str.size() && j == cmp.size();
        }

        using DecimalThreshold = std::pair<std::string, DetailedValueType>;

        const std::vector<DecimalThreshold> PositiveDecimalThresholds = {
                {"127",                  DetailedValueType::UINT_7},
                {"255",                  DetailedValueType::UINT_8},
                {"32767",                DetailedValueType::UINT_15},
                {"65535",                DetailedValueType::UINT_16},
                {"2147483647",           DetailedValueType::UINT_31},
                {"4294967295",           DetailedValueType::UINT_32},
                {"9223372036854775807",  DetailedValueType::UINT_63},
                {"18446744073709551615", DetailedValueType::UINT_64}};

        const std::vector<DecimalThreshold> NegativeDecimalThresholds = {
                {"128",                  DetailedValueType::SINT_8},
                {"32768",                DetailedValueType::SINT_16},
                {"2147483648",           DetailedValueType::SINT_32},
                {"9223372036854775808",  DetailedValueType::SINT_64}};

        DetailedValueType getDecimalNumberType(
                std::string_view str, size_t strDigits,
                const std::vector<DecimalThreshold>& thresholds)
        {
            for (auto& p : thresholds)
            {
                if (strDigits < p.first.size())
                    return p.second;
                if (strDigits == p.first.size()
                    && isLessThanOrEqualNumber(str, p.first))
                    return p.second;
            }
            return DetailedValueType::BIG_INT;
        }

        using Pow2Threshold = std::tuple<size_t, char, DetailedValueType>;

        const std::vector<Pow2Threshold> PositiveBinaryThresholds{
                {7,  '1', DetailedValueType::UINT_7},
                {8,  '1', DetailedValueType::UINT_8},
                {15, '1', DetailedValueType::UINT_15},
                {16, '1', DetailedValueType::UINT_16},
                {31, '1', DetailedValueType::UINT_31},
                {32, '1', DetailedValueType::UINT_32},
                {63, '1', DetailedValueType::UINT_63},
                {64, '1', DetailedValueType::UINT_64}};

        const std::vector<Pow2Threshold> NegativeBinaryThresholds{
                {8,  '1', DetailedValueType::SINT_8},
                {16, '1', DetailedValueType::SINT_16},
                {32, '1', DetailedValueType::SINT_32},
                {64, '1', DetailedValueType::SINT_64}};

        const std::vector<Pow2Threshold> PositiveOctalThresholds{
                {3,  '1', DetailedValueType::UINT_7},
                {3,  '3', DetailedValueType::UINT_8},
                {5,  '7', DetailedValueType::UINT_15},
                {6,  '1', DetailedValueType::UINT_16},
                {11, '1', DetailedValueType::UINT_31},
                {11, '3', DetailedValueType::UINT_32},
                {21, '7', DetailedValueType::UINT_63},
                {22, '1', DetailedValueType::UINT_64}};

        const std::vector<Pow2Threshold> NegativeOctalThresholds{
                {3,  '2', DetailedValueType::SINT_8},
                {6,  '1', DetailedValueType::SINT_16},
                {11, '2', DetailedValueType::SINT_32},
                {22, '1', DetailedValueType::SINT_64}};

        const std::vector<Pow2Threshold> PositiveHexadecimalThresholds{
                {2,  '7', DetailedValueType::UINT_7},
                {2,  'F', DetailedValueType::UINT_8},
                {4,  '7', DetailedValueType::UINT_15},
                {4,  'F', DetailedValueType::UINT_16},
                {8,  '7', DetailedValueType::UINT_31},
                {8,  'F', DetailedValueType::UINT_32},
                {16, '7', DetailedValueType::UINT_63},
                {16, 'F', DetailedValueType::UINT_64}};

        const std::vector<Pow2Threshold> NegativeHexadecimalThresholds{
                {2,  '8', DetailedValueType::SINT_8},
                {4,  '8', DetailedValueType::SINT_16},
                {8,  '8', DetailedValueType::SINT_32},
                {16, '8', DetailedValueType::SINT_64}};

        DetailedValueType getBasePowerOfTwoNumberType(
                std::string_view str,
                size_t strDigits,
                const std::vector<Pow2Threshold>& thresholds,
                bool checkSubsequentDigits)
        {
            for (auto& p : thresholds)
            {
                if (strDigits < std::get<0>(p))
                    return std::get<2>(p);
                if (strDigits > std::get<0>(p))
                    continue;
                if (str[0] < std::get<1>(p))
                    return std::get<2>(p);
                if (str[0] > std::get<1>(p))
                    continue;
                if (!checkSubsequentDigits || isAllZeros(str.substr(1)))
                    return std::get<2>(p);
            }
            return DetailedValueType::BIG_INT;
        }

        bool isLegalUnderscore(std::string_view str, size_t i)
        {
            return str[i] == '_' && 0 < i && i < str.size() - 1
                   && str[i - 1] != '_';
        }
        std::string_view skipLeadingZeros(std::string_view str)
        {
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (str[i] == '0')
                    continue;
                if (isLegalUnderscore(str, i))
                    continue;
                return str.substr(i);
            }
            return std::string_view();
        }

        DetailedValueType getBinaryNumberType(std::string_view str,
                                              bool negative)
        {
            if (str.empty())
                return DetailedValueType::INVALID;
            str = skipLeadingZeros(str);
            size_t digitCount = 0;
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 1)
                    ++digitCount;
                else if (!isLegalUnderscore(str, i))
                    return DetailedValueType::INVALID;
            }
            return getBasePowerOfTwoNumberType(
                    str, digitCount,
                    negative ? NegativeBinaryThresholds
                             : PositiveBinaryThresholds,
                    negative);
        }

        DetailedValueType getOctalNumberType(std::string_view str,
                                             bool negative)
        {
            if (str.empty())
                return DetailedValueType::INVALID;
            str = skipLeadingZeros(str);
            size_t digitCount = 0;
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 7)
                    ++digitCount;
                else if (!isLegalUnderscore(str, i))
                    return DetailedValueType::INVALID;
            }
            return getBasePowerOfTwoNumberType(
                    str, digitCount,
                    negative ? NegativeOctalThresholds
                             : PositiveOctalThresholds,
                    negative);
        }

        DetailedValueType getHexadecimalNumberType(std::string_view str,
                                                   bool negative)
        {
            if (str.empty())
                return DetailedValueType::INVALID;
            str = skipLeadingZeros(str);
            size_t digitCount = 0;
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (isHexDigit(str[i]))
                    ++digitCount;
                else if (!isLegalUnderscore(str, i))
                    return DetailedValueType::INVALID;
            }
            return getBasePowerOfTwoNumberType(
                    str, digitCount,
                    negative ? NegativeHexadecimalThresholds
                             : PositiveHexadecimalThresholds,
                    negative);
        }

        DetailedValueType getNumberType(std::string_view str, bool negative)
        {
            str = skipLeadingZeros(str);
            size_t digitCount = 0;
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (getDigit(str[i]) <= 9)
                {
                    ++digitCount;
                    continue;
                }
                if (isLegalUnderscore(str, i))
                    continue;

                if ((str[i] == '.' || str[i] == 'e' || str[i] == 'E')
                    && i != 0 && str[i - 1] != '_')
                    return getFloatingPointNumberType(str.substr(i));

                return DetailedValueType::INVALID;
            }
            if (digitCount == 0)
                return DetailedValueType::UINT_7;
            return getDecimalNumberType(
                    str, digitCount,
                    negative ? NegativeDecimalThresholds
                             : PositiveDecimalThresholds);
        }
    }
}
