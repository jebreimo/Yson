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
        constexpr size_t getUtf8EncodedLength(char32_t c)
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
        size_t encodeUtf8(char32_t chr, size_t chrLength, OutputIt& it)
        {
            if (chrLength == 1)
            {
                *it++ = char(chr);
            }
            else if (chrLength != 0)
            {
                size_t shift = (chrLength - 1) * 6;
                *it++ = char((0xFFu << (8 - chrLength)) | (chr >> shift));
                for (size_t i = 1; i < chrLength; i++)
                {
                    shift -= 6;
                    *it++ = char(0x80u | ((chr >> shift) & 0x3Fu));
                }
            }
            return chrLength;
        }
    }

    Utf8Encoder::Utf8Encoder()
        : EncoderBase(Encoding::UTF_8)
    {}

    size_t Utf8Encoder::getEncodedSize(const char32_t* src, size_t srcSize)
    {
        size_t result = 0;
        for (size_t i = 0; i < srcSize; ++i)
            result += Detail::getUtf8EncodedLength(src[i]);
        return result;
    }

    std::pair<size_t, size_t>
    Utf8Encoder::encode(const char32_t* src, size_t srcSize,
                        void* dst, size_t dstSize)
    {
        auto cdst = static_cast<char*>(dst);
        size_t bytesWritten = 0;
        for (size_t i = 0; i < srcSize; ++i)
        {
            size_t length = Detail::getUtf8EncodedLength(src[i]);
            if (length > dstSize - bytesWritten)
                return {i, bytesWritten};
            Detail::encodeUtf8(src[i], length, cdst);
            bytesWritten += length;
        }
        return {srcSize, bytesWritten};
    }

    size_t Utf8Encoder::encode(const char32_t* src, size_t srcSize,
                               std::string& dst)
    {
        auto out = back_inserter(dst);
        for (size_t i = 0; i < srcSize; ++i)
        {
            auto n = Detail::getUtf8EncodedLength(src[i]);
            Detail::encodeUtf8(src[i], n, out);
        }
        return srcSize;
    }
}
