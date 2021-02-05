//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Utf8Decoder.hpp"

namespace Yconvert
{
    namespace Detail
    {
        char32_t nextUtf8Value(const char*& it, const char* end)
        {
            if (it == end)
                return INVALID_CHAR;

            if ((uint8_t(*it) & 0x80u) == 0)
                return uint8_t(*it++);

            char32_t result;
            int n = 0;
            if ((uint8_t(*it) & 0xE0u) == 0xC0)
            {
                result = uint8_t(*it++) & 0x1Fu;
                n = 1;
            }
            else if ((uint8_t(*it) & 0xF0u) == 0xE0)
            {
                result = uint8_t(*it++) & 0x0Fu;
                n = 2;
            }
            else if ((uint8_t(*it) & 0xF8u) == 0xF0)
            {
                result = uint8_t(*it++) & 0x07u;
                n = 3;
            }
            else
            {
                return INVALID_CHAR;
            }

            if (n > std::distance(it, end))
            {
                --it;
                return INVALID_CHAR;
            }

            for (int i = 0; i < n; ++i)
            {
                result <<= 6u;
                if (auto c = uint8_t(*it); (c & 0xC0u) == 0x80)
                {
                    result |= c & 0x3Fu;
                }
                else
                {
                    for (int j = 0; j <= i; ++j)
                        --it;
                    return INVALID_CHAR;
                }
                ++it;
            }

            return result;
        }

        bool skipNextUtf8Value(const char*& it, const char* end)
        {
            if (it == end)
                return false;

            auto c = uint8_t(*it++);

            if ((c & 0x80u) == 0)
                return true;

            uint32_t n;
            if ((c & 0xE0u) == 0xC0)
                n = 1;
            else if ((c & 0xF0u) == 0xE0)
                n = 2;
            else if ((c & 0xF8u) == 0xF0)
                n = 3;
            else
                n = UINT32_MAX;

            while (it != end && (uint8_t(*it) & 0xC0u) == 0x80 && n-- > 0)
                ++it;

            return true;
        }
    }

    Utf8Decoder::Utf8Decoder()
        : DecoderBase(Encoding::UTF_8)
    {}

    size_t Utf8Decoder::skipCharacter(const void* src, size_t srcSize) const
    {
        auto cSrc = static_cast<const char*>(src);
        auto initialSrc = cSrc;
        Detail::skipNextUtf8Value(cSrc, cSrc + srcSize);
        return size_t(cSrc - initialSrc);
    }

    std::pair<size_t, size_t>
    Utf8Decoder::doDecode(const void* src, size_t srcSize,
                          char32_t* dst, size_t dstSize) const
    {
        auto cSrc = static_cast<const char*>(src);
        auto initialSrc = cSrc;
        auto initialDst = dst;
        auto srcEnd = cSrc + srcSize;
        auto dstEnd = dst + dstSize;
        while (dst != dstEnd)
        {
            auto value = Detail::nextUtf8Value(cSrc, srcEnd);
            if (value == INVALID_CHAR)
                break;
            *dst++ = value;
        }
        return {size_t(cSrc - initialSrc), size_t(dst - initialDst)};
    }
}
