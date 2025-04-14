//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Utf8Decoder.hpp"

namespace Yconvert
{
    namespace Detail
    {
        char32_t next_utf8_value(const char*& it, const char* end)
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

        bool skip_next_utf8_value(const char*& it, const char* end)
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

    size_t Utf8Decoder::skip_character(const void* src, size_t src_size) const
    {
        auto c_src = static_cast<const char*>(src);
        auto initial_src = c_src;
        Detail::skip_next_utf8_value(c_src, c_src + src_size);
        return size_t(c_src - initial_src);
    }

    std::pair<size_t, size_t>
    Utf8Decoder::do_decode(const void* src, size_t src_size,
                           char32_t* dst, size_t dst_size) const
    {
        auto c_src = static_cast<const char*>(src);
        auto initial_src = c_src;
        auto initial_dst = dst;
        auto src_end = c_src + src_size;
        auto dst_end = dst + dst_size;
        while (dst != dst_end)
        {
            auto value = Detail::next_utf8_value(c_src, src_end);
            if (value == INVALID_CHAR)
                break;
            *dst++ = value;
        }
        return {size_t(c_src - initial_src), size_t(dst - initial_dst)};
    }
}
