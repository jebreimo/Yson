//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "DecoderBase.hpp"

namespace Yconvert
{
    namespace Detail
    {
        template <bool SWAP_BYTES, typename BiIt>
        char32_t next_utf32_word(BiIt& it, BiIt end)
        {
            union U {char32_t c; uint8_t b[4];} u;
            for (int i = 0; i < 4; ++i)
            {
                if (it == end)
                    return INVALID_CHAR;
                u.b[SWAP_BYTES ? 3 - i : i] = uint8_t(*it++);
            }
            return u.c;
        }

        template <typename FwdIt>
        bool skip_next_utf32_code_point(FwdIt& it, FwdIt end)
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

        template <bool SWAP_BYTES, typename BiIt>
        char32_t next_utf32_code_point(BiIt& it, BiIt end)
        {
            auto first = it;
            auto chr = Detail::next_utf32_word<SWAP_BYTES>(it, end);
            if (chr == INVALID_CHAR)
                it = first;
            return chr;
        }
    }

    template <bool SWAP_BYTES>
    class Utf32Decoder : public DecoderBase
    {
    public:
        Utf32Decoder()
            : DecoderBase(IS_BIG_ENDIAN == SWAP_BYTES
                          ? Encoding::UTF_32_LE
                          : Encoding::UTF_32_BE)
        {}

    protected:
        size_t skip_character(const void* src, size_t src_size) const final
        {
            auto c_src = static_cast<const char*>(src);
            auto initial_src = c_src;
            Detail::skip_next_utf32_code_point(c_src, c_src + src_size);
            return size_t(c_src - initial_src);
        }

        std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const final
        {
            auto c_src = static_cast<const char*>(src);
            auto initial_src = c_src;
            auto initial_dst = dst;
            auto src_end = c_src + src_size;
            auto dst_end = dst + dst_size;
            while (dst != dst_end)
            {
                auto value = Detail::next_utf32_code_point<SWAP_BYTES>(c_src, src_end);
                if (value == INVALID_CHAR)
                    break;
                *dst++ = value;
            }
            return {size_t(c_src - initial_src), size_t(dst - initial_dst)};
        }
    };

    using Utf32BEDecoder = Utf32Decoder<IS_LITTLE_ENDIAN>;
    using Utf32LEDecoder = Utf32Decoder<IS_BIG_ENDIAN>;
}
