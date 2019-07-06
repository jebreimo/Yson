//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iterator>
#include "Ystring/Utilities/Endian.hpp"
#include "Ystring/Utilities/Union16.hpp"

namespace Ystring { namespace Encodings
{
    using Ystring::Utilities::swapEndianness;
    using Ystring::Utilities::Union16;

    template <bool SwapBytes, typename OutIt>
    OutIt addUtf16(OutIt out, char32_t codePoint)
    {
        if (codePoint <= 0xFFFF)
        {
            char16_t word = (char16_t)codePoint;
            swapEndianness<SwapBytes>(word);
            *out = word;
            return ++out;
        }
        else
        {
            codePoint -= 0x10000;
            char16_t word1 = char16_t(0xD800 | (codePoint >> 10));
            char16_t word2 = char16_t(0xDC00 | (codePoint & 0x3FF));
            swapEndianness<SwapBytes>(word1);
            swapEndianness<SwapBytes>(word2);
            *out = word1;
            *++out = word2;
            return ++out;
        }
    }

    template <typename OutIt>
    OutIt addUtf16(OutIt out, char32_t codePoint)
    {
        return addUtf16<false>(out, codePoint);
    }

    template <typename OutIt>
    OutIt addUtf16LE(OutIt out, char32_t codePoint)
    {
        return addUtf16<IsBigEndian>(out, codePoint);
    }

    template <typename OutIt>
    OutIt addUtf16BE(OutIt out, char32_t codePoint)
    {
        return addUtf16<IsLittleEndian>(out, codePoint);
    }

    template <bool SwapBytes, typename OutIt>
    OutIt addUtf16AsBytes(OutIt out, char32_t codePoint)
    {
        if (codePoint <= 0xFFFF)
        {
            Union16 word((char16_t)codePoint);
            swapEndianness<SwapBytes>(word);
            *out++ = word.u8[0];
            *out++ = word.u8[1];
            return out;
        }
        else
        {
            codePoint -= 0x10000;
            Union16 word1(char16_t(0xD800 | (codePoint >> 10)));
            Union16 word2(char16_t(0xDC00 | (codePoint & 0x3FF)));
            swapEndianness<SwapBytes>(word1);
            swapEndianness<SwapBytes>(word2);
            *out++ = word1.u8[0];
            *out++ = word1.u8[1];
            *out++ = word2.u8[0];
            *out++ = word2.u8[1];
            return out;
        }
    }

    template <bool SwapBytes, typename FwdIt, typename ChrType>
    size_t encodeUtf16(FwdIt& begin, FwdIt end, char32_t codePoint, ChrType)
    {
        if (codePoint <= 0xFFFF)
        {
            if (begin == end)
                return 0;
            Union16 word((char16_t)codePoint);
            swapEndianness<SwapBytes>(word);
            *begin++ = word.u16;
            return 1;
        }
        else
        {
            if (std::distance(begin, end) < 2)
                return 0;
            codePoint -= 0x10000;
            Union16 word1(char16_t(0xD800 | (codePoint >> 10)));
            Union16 word2(char16_t(0xDC00 | (codePoint & 0x3FF)));
            swapEndianness<SwapBytes>(word1);
            swapEndianness<SwapBytes>(word2);
            *begin++ = word1.u16;
            *begin++ = word2.u16;
            return 2;
        }
    }

    template <bool SwapBytes, typename FwdIt>
    size_t encodeUtf16(FwdIt& begin, FwdIt end, char32_t codePoint, uint8_t)
    {
        if (codePoint <= 0xFFFF)
        {
            if (std::distance(begin, end) < 2)
                return 0;
            Union16 word((char16_t)codePoint);
            swapEndianness<SwapBytes>(word);
            *begin++ = word.i8[0];
            *begin++ = word.i8[1];
            return 2;
        }
        else
        {
            if (std::distance(begin, end) < 4)
                return 0;
            codePoint -= 0x10000;
            Union16 word1((char16_t)(0xD800 | (codePoint >> 10)));
            Union16 word2((char16_t)(0xDC00 | (codePoint & 0x3FF)));
            swapEndianness<SwapBytes>(word1);
            swapEndianness<SwapBytes>(word2);
            *begin++ = word1.i8[0];
            *begin++ = word1.i8[1];
            *begin++ = word2.i8[0];
            *begin++ = word2.i8[1];
            return 4;
        }
    }

    template <bool SwapBytes, typename FwdIt>
    size_t encodeUtf16(FwdIt& begin, FwdIt end, char32_t codePoint, char)
    {
        return encodeUtf16<SwapBytes>(begin, end, codePoint, uint8_t());
    }

    template <typename FwdIt>
    size_t encodeUtf16(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return encodeUtf16<false>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }

    template <typename FwdIt>
    size_t encodeUtf16LE(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return encodeUtf16<IsBigEndian>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }

    template <typename FwdIt>
    size_t encodeUtf16BE(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return encodeUtf16<IsLittleEndian>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }
}}
