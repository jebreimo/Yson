//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CodePageDecoder.hpp"

#ifdef YCONVERT_ENABLE_CODE_PAGES

namespace Yconvert
{
    CodePageDecoder::CodePageDecoder(Encoding encoding,
                                     const CodePageRange* ranges,
                                     size_t ranges_size)
        : DecoderBase(encoding)
    {
        std::fill(std::begin(chars_), std::end(chars_), INVALID_CHAR);
        char32_t upper = 0;
        for (size_t i = 0; i < ranges_size; ++i)
        {
            if (ranges[i].start_index != 0xFF || ranges[i].length != 0xFF)
            {
                char32_t ch = upper | ranges[i].start_code_point;
                for (size_t j = 0; j <= ranges[i].length; ++j)
                    chars_[ranges[i].start_index + j] = ch++;
                upper = 0;
            }
            else
            {
                upper = ranges[i].start_code_point << 16u;
            }
        }
    }

    size_t CodePageDecoder::skip_character(const void*, size_t src_size) const
    {
        return src_size ? 1 : 0;
    }

    std::pair<size_t, size_t>
    CodePageDecoder::do_decode(const void* src, size_t src_size,
                               char32_t* dst, size_t dst_size) const
    {
        auto csrc = static_cast<const uint8_t*>(src);
        auto count = std::min(src_size, dst_size);
        for (size_t i = 0; i < count; ++i)
            dst[i] = chars_[csrc[i]];
        return {count, count};
    }
}

#endif
