//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-03.
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
        char32_t nextUtf32Word(BiIt& it, BiIt end)
        {
            union U {char32_t c; uint8_t b[4];} u;
            for (int i = 0; i < 4; ++i)
            {
                if (it == end)
                    return INVALID_CHAR;
                u.b[SwapBytes ? 3 - i : i] = uint8_t(*it++);
            }
            return u.c;
        }

        template <typename FwdIt>
        bool skipNextUtf32CodePoint(FwdIt& it, FwdIt end)
        {
            if (it == end)
                return false;
            for (int i = 0; i < 4; ++i)
            {
                if (++it == end)
                    break;
            }
            return true;
        }

        template <bool SwapBytes, typename BiIt>
        char32_t nextUtf32CodePoint(BiIt& it, BiIt end)
        {
            auto first = it;
            auto chr = Detail::nextUtf32Word<SwapBytes>(it, end);
            if (chr == INVALID_CHAR)
                it = first;
            return chr;
        }
    }

    template <bool SwapBytes>
    class Utf32Decoder : public DecoderBase
    {
    public:
        Utf32Decoder()
            : DecoderBase(IsBigEndian == SwapBytes
                          ? Encoding::UTF_32_LE
                          : Encoding::UTF_32_BE)
        {}

    protected:
        size_t skipCharacter(const void* src, size_t srcSize) const final
        {
            auto cSrc = static_cast<const char*>(src);
            auto initialSrc = cSrc;
            Detail::skipNextUtf32CodePoint(cSrc, cSrc + srcSize);
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
                auto value = Detail::nextUtf32CodePoint<SwapBytes>(cSrc, srcEnd);
                if (value == INVALID_CHAR)
                    break;
                *dst++ = value;
            }
            return {size_t(cSrc - initialSrc), size_t(dst - initialDst)};
        }
    };

    using Utf32BEDecoder = Utf32Decoder<IsLittleEndian>;
    using Utf32LEDecoder = Utf32Decoder<IsBigEndian>;
}
