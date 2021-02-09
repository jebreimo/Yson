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
        template <bool SwapBytes, typename OutIt>
        void addBytes(char32_t c, OutIt& out)
        {
            union {char32_t c; char b[4];} u = {swapEndianness<SwapBytes>(c)};
            *out++ = u.b[0];
            *out++ = u.b[1];
            *out++ = u.b[2];
            *out++ = u.b[3];
        }
    }

    template <bool SwapBytes>
    class Utf32Encoder : public EncoderBase
    {
    public:
        Utf32Encoder()
            : EncoderBase(IsBigEndian == SwapBytes
                          ? Encoding::UTF_32_LE
                          : Encoding::UTF_32_BE)
        {}

        size_t getEncodedSize(const char32_t* src, size_t srcSize) override
        {
            size_t result = 0;
            for (size_t i = 0; i < srcSize; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                    result += 4;
            }
            return result;
        }

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t srcSize,
               void* dst, size_t dstSize) override
        {
            auto cdst = static_cast<char*>(dst);
            size_t bytesWritten = 0;
            for (size_t i = 0; i < srcSize; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                {
                    bytesWritten += 4;
                    if (dstSize < bytesWritten)
                        return {i, bytesWritten - 4};
                    Detail::addBytes<SwapBytes>(src[i], cdst);
                    cdst += 4;
                }
            }
            return {srcSize, bytesWritten};
        }

        size_t encode(const char32_t* src, size_t srcSize,
                      std::string& dst) override
        {
            auto out = back_inserter(dst);
            for (size_t i = 0; i < srcSize; ++i)
            {
                if (src[i] <= UNICODE_MAX)
                    Detail::addBytes<SwapBytes>(src[i], out);
            }
            return srcSize;
        }
    };

    typedef Utf32Encoder<IsLittleEndian> Utf32BEEncoder;
    typedef Utf32Encoder<IsBigEndian> Utf32LEEncoder;
}
