//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "EncoderBase.hpp"
#include <iterator>

namespace Yconvert
{
    namespace Detail
    {
        template <bool SWAP_BYTES, typename OutIt>
        void add_bytes(char32_t c, OutIt& out)
        {
            union {char32_t c; char b[4];} u = {swap_endianness<SWAP_BYTES>(c)};
            *out++ = u.b[0];
            *out++ = u.b[1];
            *out++ = u.b[2];
            *out++ = u.b[3];
        }
    }

    template <bool SWAP_BYTES>
    class Utf32Encoder : public EncoderBase
    {
    public:
        Utf32Encoder()
            : EncoderBase(IS_BIG_ENDIAN == SWAP_BYTES
                          ? Encoding::UTF_32_LE
                          : Encoding::UTF_32_BE)
        {}

        size_t get_encoded_size(const char32_t* src, size_t src_size) override
        {
            size_t result = 0;
            for (size_t i = 0; i < src_size; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                    result += 4;
            }
            return result;
        }

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) override
        {
            auto cdst = static_cast<char*>(dst);
            size_t bytes_written = 0;
            for (size_t i = 0; i < src_size; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                {
                    bytes_written += 4;
                    if (dst_size < bytes_written)
                        return {i, bytes_written - 4};
                    Detail::add_bytes<SWAP_BYTES>(src[i], cdst);
                }
            }
            return {src_size, bytes_written};
        }

        size_t encode(const char32_t* src, size_t src_size,
                      std::string& dst) override
        {
            auto out = back_inserter(dst);
            for (size_t i = 0; i < src_size; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                    Detail::add_bytes<SWAP_BYTES>(src[i], out);
            }
            return src_size;
        }
    };

    typedef Utf32Encoder<IS_LITTLE_ENDIAN> Utf32BEEncoder;
    typedef Utf32Encoder<IS_BIG_ENDIAN> Utf32LEEncoder;
}
