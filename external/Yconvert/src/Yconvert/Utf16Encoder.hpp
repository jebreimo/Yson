//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "EncoderBase.hpp"

namespace Yconvert
{
    namespace Detail
    {
        template <bool SwapBytes, typename OutIt>
        void addBytes(char16_t c, OutIt& out)
        {
            union {char16_t c; char b[2];} u = {swapEndianness<SwapBytes>(c)};
            *out++ = u.b[0];
            *out++ = u.b[1];
        }

        template <bool SwapBytes, typename OutIt>
        void addUtf16Bytes(char32_t codePoint, OutIt out)
        {
            if (codePoint <= 0xFFFF)
            {
                Detail::addBytes<SwapBytes>(char16_t(codePoint), out);
            }
            else if (codePoint <= UNICODE_MAX)
            {
                codePoint -= 0x10000;
                auto word1 = char16_t(0xD800u | (codePoint >> 10u));
                auto word2 = char16_t(0xDC00u | (codePoint & 0x3FFu));
                Detail::addBytes<SwapBytes>(word1, out);
                Detail::addBytes<SwapBytes>(word2, out);
            }
        }

        template <bool SwapBytes, typename T>
        size_t encodeUtf16(char32_t codePoint, T* data, size_t n)
        {
            if (codePoint <= 0xFFFF)
            {
                if (n < 2)
                    return 0;
                Detail::addBytes<SwapBytes>(char16_t(codePoint), data);
                return 2;
            }
            else if (codePoint <= UNICODE_MAX)
            {
                if (n < 4)
                    return 0;
                codePoint -= 0x10000;
                auto word1 = char16_t(0xD800u | (codePoint >> 10u));
                auto word2 = char16_t(0xDC00u | (codePoint & 0x3FFu));
                Detail::addBytes<SwapBytes>(word1, data);
                Detail::addBytes<SwapBytes>(word2, data);
                return 4;
            }
            return 0;
        }
    }

    template <bool SwapBytes>
    class Utf16Encoder : public EncoderBase
    {
    public:
        Utf16Encoder()
            : EncoderBase(IsBigEndian == SwapBytes
                          ? Encoding::UTF_16_LE
                          : Encoding::UTF_16_BE)
        {}

        size_t getEncodedSize(const char32_t* src, size_t srcSize) override
        {
            size_t result = 0;
            for (size_t i = 0; i < srcSize; ++i)
            {
                if (src[i] <= 0xFFFF)
                    result +=  2;
                else if (src[i] <= UNICODE_MAX)
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
                auto n = Detail::encodeUtf16<SwapBytes>(
                    src[i], cdst + bytesWritten, dstSize - bytesWritten);
                if (n == 0 && src[i] <= UNICODE_MAX)
                    return {i, bytesWritten};
                bytesWritten += n;
            }
            return {srcSize, bytesWritten};
        }

        size_t encode(const char32_t* src, size_t srcSize,
                      std::string& dst) override
        {
            auto out = back_inserter(dst);
            for (size_t i = 0; i < srcSize; ++i)
                Detail::addUtf16Bytes<SwapBytes>(src[i], out);
            return srcSize;
        }
    };

    using Utf16BEEncoder = Utf16Encoder<IsLittleEndian>;
    using Utf16LEEncoder = Utf16Encoder<IsBigEndian>;
}
