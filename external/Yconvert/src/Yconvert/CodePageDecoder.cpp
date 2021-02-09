//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CodePageDecoder.hpp"

#ifdef YCONVERT_ENABLE_CODE_PAGES

namespace Yconvert
{
    CodePageDecoder::CodePageDecoder(Encoding encoding,
                                     const CodePageRange* ranges,
                                     size_t rangesSize)
        : DecoderBase(encoding)
    {
        std::fill(std::begin(m_Chars), std::end(m_Chars), INVALID_CHAR);
        char32_t upper = 0;
        for (size_t i = 0; i < rangesSize; ++i)
        {
            if (ranges[i].startIndex != 0xFF || ranges[i].length != 0xFF)
            {
                char32_t ch = upper | ranges[i].startCodePoint;
                for (size_t j = 0; j <= ranges[i].length; ++j)
                    m_Chars[ranges[i].startIndex + j] = ch++;
                upper = 0;
            }
            else
            {
                upper = ranges[i].startCodePoint << 16u;
            }
        }
    }

    size_t CodePageDecoder::skipCharacter(const void* /*src*/, size_t srcSize) const
    {
        return srcSize ? 1 : 0;
    }

    std::pair<size_t, size_t>
    CodePageDecoder::doDecode(const void* src, size_t srcSize, char32_t* dst, size_t dstSize) const
    {
        auto csrc = static_cast<const uint8_t*>(src);
        auto count = std::min(srcSize, dstSize);
        for (size_t i = 0; i < count; ++i)
            dst[i] = m_Chars[csrc[i]];
        return {count, count};
    }
}

#endif
