//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "DecoderBase.hpp"

namespace Yconvert
{
    namespace Detail
    {
        template <bool SwapBytes, typename BiIt>
        char32_t nextUtf16Word(BiIt& it, BiIt end)
        {
            if (it == end)
                return INVALID_CHAR;

            union U {char16_t c; uint8_t b[2];} u;
            u.b[SwapBytes ? 1 : 0] = uint8_t(*it++);
            if (it == end)
                return INVALID_CHAR;
            u.b[SwapBytes ? 0 : 1] = uint8_t(*it++);
            return u.c;
        }

        template <bool SwapBytes, typename FwdIt>
        bool skipNextUtf16CodePoint(FwdIt& it, FwdIt end)
        {
            if (it == end)
                return false;
            auto chr = nextUtf16Word<SwapBytes>(it, end);
            if (chr < 0xD800 || 0xDC00 <= chr)
                return true;

            auto pos = it;
            auto chr2 = nextUtf16Word<SwapBytes>(it, end);
            if (chr2 != INVALID_CHAR && (chr2 < 0xDC00 || 0xE000 <= chr2))
                it = pos;
            return true;
        }

        template <bool SwapBytes, typename BiIt>
        char32_t nextUtf16CodePoint(BiIt& it, BiIt end)
        {
            auto first = it;
            auto chr = Detail::nextUtf16Word<SwapBytes>(it, end);
            if (chr == INVALID_CHAR)
            {
                it = first;
                return INVALID_CHAR;
            }

            if (chr < 0xD800 || 0xE000 <= chr)
                return chr;

            if (0xDC00 <= chr)
            {
                it = first;
                return INVALID_CHAR;
            }

            auto chr2 = Detail::nextUtf16Word<SwapBytes>(it, end);
            if (chr2 == INVALID_CHAR || chr2 < 0xDC00 || 0xE000 <= chr2)
            {
                it = first;
                return INVALID_CHAR;
            }

            return char32_t(((chr & 0x3FFu) << 10u) + (chr2 & 0x3FFu) + 0x10000);
        }
    }

    template <bool SwapBytes>
    class Utf16Decoder : public DecoderBase
    {
    public:
        Utf16Decoder() : DecoderBase(IsBigEndian == SwapBytes
                                     ? Encoding::UTF_16_LE
                                     : Encoding::UTF_16_BE)
        {}
    protected:
        size_t skipCharacter(const void* src, size_t srcSize) const final
        {
            auto cSrc = static_cast<const char*>(src);
            auto initialSrc = cSrc;
            Detail::skipNextUtf16CodePoint<SwapBytes>(cSrc, cSrc + srcSize);
            return size_t(cSrc - initialSrc);
        }

        std::pair<size_t, size_t>
        doDecode(const void* src, size_t srcSize,
                 char32_t* dst, size_t dstSize) const final
        {
            auto cSrc = static_cast<const char*>(src);
            auto initialSrc = cSrc;
            auto initialDst = dst;
            auto srcEnd = cSrc + srcSize;
            auto dstEnd = dst + dstSize;
            while (dst != dstEnd)
            {
                auto value = Detail::nextUtf16CodePoint<SwapBytes>(cSrc, srcEnd);
                if (value == INVALID_CHAR)
                    break;
                *dst++ = value;
            }
            return {size_t(cSrc - initialSrc), size_t(dst - initialDst)};
        }
    };

    using Utf16BEDecoder = Utf16Decoder<IsLittleEndian>;
    using Utf16LEDecoder = Utf16Decoder<IsBigEndian>;
}
