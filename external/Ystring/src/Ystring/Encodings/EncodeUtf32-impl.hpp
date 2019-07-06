//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-11
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iterator>
#include "Ystring/Utilities/Endian.hpp"
#include "Ystring/Utilities/Union32.hpp"

namespace Ystring { namespace Encodings
{
    using Ystring::Utilities::swapEndianness;

    namespace Detail
    {
        template <bool SwapBytes, typename FwdIt, typename ChrType>
        size_t encodeUtf32(FwdIt& begin, FwdIt end, char32_t codePoint,
                           ChrType)
        {
            if (begin == end)
                return 0;
            swapEndianness<SwapBytes>(codePoint);
            *begin++ = codePoint;
            return 1;
        }

        template <bool SwapBytes, typename FwdIt>
        size_t encodeUtf32(FwdIt& begin, FwdIt end, char32_t codePoint,
                           uint8_t)
        {
            if (std::distance(begin, end) < 4)
                return 0;
            Ystring::Utilities::Union32 word(codePoint);
            swapEndianness<SwapBytes>(word);
            *begin++ = word.i8[0];
            *begin++ = word.i8[1];
            *begin++ = word.i8[2];
            *begin++ = word.i8[3];
            return 4;
        }

        template <bool SwapBytes, typename FwdIt>
        size_t encodeUtf32(FwdIt& begin, FwdIt end, char32_t codePoint, char)
        {
            return encodeUtf32<SwapBytes>(begin, end, codePoint, uint8_t());
        }
    }

    template <bool SwapBytes, typename OutIt>
    OutIt addUtf32(OutIt out, char32_t codePoint)
    {
        swapEndianness<SwapBytes>(codePoint);
        *out = codePoint;
        return ++out;
    }

    template <typename OutIt>
    OutIt addUtf32(OutIt out, char32_t codePoint)
    {
        return addUtf32<false>(out, codePoint);
    }

    template <typename OutIt>
    OutIt addUtf32LE(OutIt out, char32_t codePoint)
    {
        return addUtf32<IsBigEndian>(out, codePoint);
    }

    template <typename OutIt>
    OutIt addUtf32BE(OutIt out, char32_t codePoint)
    {
        return addUtf32<IsLittleEndian>(out, codePoint);
    }

    template<bool SwapBytes, typename OutIt>
    OutIt addUtf32AsBytes(OutIt out, char32_t codePoint)
    {
        Ystring::Utilities::Union32 word(codePoint);
        swapEndianness<SwapBytes>(word);
        *out++ = word.i8[0];
        *out++ = word.i8[1];
        *out++ = word.i8[2];
        *out++ = word.i8[3];
        return out;
    }

    template <typename FwdIt>
    size_t encodeUtf32(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return Detail::encodeUtf32<false>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }

    template <typename FwdIt>
    size_t encodeUtf32LE(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return Detail::encodeUtf32<IsBigEndian>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }

    template <typename FwdIt>
    size_t encodeUtf32BE(FwdIt& begin, FwdIt end, char32_t codePoint)
    {
        return Detail::encodeUtf32<IsLittleEndian>(
                begin, end, codePoint,
                typename std::iterator_traits<FwdIt>::value_type());
    }
}}
