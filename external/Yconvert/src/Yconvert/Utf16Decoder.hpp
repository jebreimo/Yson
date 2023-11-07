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
        template <bool SWAP_BYTES, typename BiIt>
        char32_t next_utf16_word(BiIt& it, BiIt end)
        {
            if (it == end)
                return INVALID_CHAR;

            union U {char16_t c; uint8_t b[2];} u;
            u.b[SWAP_BYTES ? 1 : 0] = uint8_t(*it++);
            if (it == end)
                return INVALID_CHAR;
            u.b[SWAP_BYTES ? 0 : 1] = uint8_t(*it++);
            return u.c;
        }

        template <bool SWAP_BYTES, typename FwdIt>
        bool skip_next_utf16_code_point(FwdIt& it, FwdIt end)
        {
            if (it == end)
                return false;
            auto chr = next_utf16_word<SWAP_BYTES>(it, end);
            if (chr < 0xD800 || 0xDC00 <= chr)
                return true;

            auto pos = it;
            auto chr2 = next_utf16_word<SWAP_BYTES>(it, end);
            if (chr2 != INVALID_CHAR && (chr2 < 0xDC00 || 0xE000 <= chr2))
                it = pos;
            return true;
        }

        template <bool SWAP_BYTES, typename BiIt>
        char32_t next_utf16_code_point(BiIt& it, BiIt end)
        {
            auto first = it;
            auto chr = Detail::next_utf16_word<SWAP_BYTES>(it, end);
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

            auto chr2 = Detail::next_utf16_word<SWAP_BYTES>(it, end);
            if (chr2 == INVALID_CHAR || chr2 < 0xDC00 || 0xE000 <= chr2)
            {
                it = first;
                return INVALID_CHAR;
            }

            return char32_t(((chr & 0x3FFu) << 10u) + (chr2 & 0x3FFu) + 0x10000);
        }
    }

    template <bool SWAP_BYTES>
    class Utf16Decoder : public DecoderBase
    {
    public:
        Utf16Decoder() : DecoderBase(IS_BIG_ENDIAN == SWAP_BYTES
                                     ? Encoding::UTF_16_LE
                                     : Encoding::UTF_16_BE)
        {}
    protected:
        size_t skip_character(const void* src, size_t src_size) const final
        {
            auto csrc = static_cast<const char*>(src);
            auto initial_src = csrc;
            Detail::skip_next_utf16_code_point<SWAP_BYTES>(csrc, csrc + src_size);
            return size_t(csrc - initial_src);
        }

        std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const final
        {
            auto csrc = static_cast<const char*>(src);
            auto initial_src = csrc;
            auto initial_dst = dst;
            auto src_end = csrc + src_size;
            auto dst_end = dst + dst_size;
            while (dst != dst_end)
            {
                auto value = Detail::next_utf16_code_point<SWAP_BYTES>(csrc, src_end);
                if (value == INVALID_CHAR)
                    break;
                *dst++ = value;
            }
            return {size_t(csrc - initial_src), size_t(dst - initial_dst)};
        }
    };

    using Utf16BEDecoder = Utf16Decoder<IS_LITTLE_ENDIAN>;
    using Utf16LEDecoder = Utf16Decoder<IS_BIG_ENDIAN>;
}
