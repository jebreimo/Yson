//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-06
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Utf8Encoder.hpp"

#include <iterator>

namespace Yconvert
{
    namespace Detail
    {
        constexpr size_t get_utf8_encoded_length(char32_t c)
        {
            if (c < 0x80u)
                return 1;
            else if (c < 0x800u)
                return 2;
            else if (c < 0x10000u)
                return 3;
            else if (c <= UNICODE_MAX)
                return 4;
            else
                return 0;
        }

        template <typename OutputIt>
        size_t encode_utf8(char32_t chr, size_t chr_length, OutputIt& it)
        {
            if (chr_length == 1)
            {
                *it++ = char(chr);
            }
            else if (chr_length != 0)
            {
                size_t shift = (chr_length - 1) * 6;
                *it++ = char((0xFFu << (8 - chr_length)) | (chr >> shift));
                for (size_t i = 1; i < chr_length; i++)
                {
                    shift -= 6;
                    *it++ = char(0x80u | ((chr >> shift) & 0x3Fu));
                }
            }
            return chr_length;
        }
    }

    Utf8Encoder::Utf8Encoder()
        : EncoderBase(Encoding::UTF_8)
    {}

    size_t Utf8Encoder::get_encoded_size(const char32_t* src, size_t src_size)
    {
        size_t result = 0;
        for (size_t i = 0; i < src_size; ++i)
            result += Detail::get_utf8_encoded_length(src[i]);
        return result;
    }

    std::pair<size_t, size_t>
    Utf8Encoder::encode(const char32_t* src, size_t src_size,
                        void* dst, size_t dst_size)
    {
        auto cdst = static_cast<char*>(dst);
        size_t bytes_written = 0;
        for (size_t i = 0; i < src_size; ++i)
        {
            size_t length = Detail::get_utf8_encoded_length(src[i]);
            if (length > dst_size - bytes_written)
                return {i, bytes_written};
            Detail::encode_utf8(src[i], length, cdst);
            bytes_written += length;
        }
        return {src_size, bytes_written};
    }

    size_t Utf8Encoder::encode(const char32_t* src, size_t src_size,
                               std::string& dst)
    {
        auto out = back_inserter(dst);
        for (size_t i = 0; i < src_size; ++i)
        {
            auto n = Detail::get_utf8_encoded_length(src[i]);
            Detail::encode_utf8(src[i], n, out);
        }
        return src_size;
    }
}
