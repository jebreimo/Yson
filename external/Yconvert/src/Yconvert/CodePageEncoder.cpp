//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-14.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CodePageEncoder.hpp"

#ifdef YCONVERT_ENABLE_CODE_PAGES

#include <algorithm>
#include <optional>
#include "Yconvert/ConversionException.hpp"

namespace Yconvert
{
    namespace
    {
        std::optional<char>
        find_char(const std::vector<CodePointMapRange>& ranges, char32_t c)
        {
            auto it = std::upper_bound(
                ranges.begin(), ranges.end(),
                CodePointMapRange{c, 0, 0},
                [](auto& a, auto& b){return a.code_point < b.code_point;});
            if (it != ranges.begin())
            {
                --it;
                auto offset = c - it->code_point;
                if (offset <= it->length)
                    return char(it->index + offset);
            }
            return {};
        }
    }

    CodePageEncoder::CodePageEncoder(Encoding encoding,
                                     const CodePageRange* ranges,
                                     size_t ranges_size)
        : EncoderBase(encoding)
    {
        ranges_.reserve(ranges_size);
        char32_t upper = 0;
        for (size_t i = 0; i < ranges_size; ++i)
        {
            if (ranges[i].start_index != 0xFF || ranges[i].length != 0xFF)
            {
                char32_t cp = upper | ranges[i].start_code_point;
                ranges_.push_back({cp, ranges[i].start_index, ranges[i].length});
                upper = 0;
            }
            else
            {
                upper = ranges[i].start_code_point << 16u;
            }
        }
        if (find_char(ranges_, REPLACEMENT_CHARACTER))
            EncoderBase::set_replacement_character(REPLACEMENT_CHARACTER);
        else if (find_char(ranges_, '?'))
            EncoderBase::set_replacement_character('?');
        else if (find_char(ranges_, ' '))
            EncoderBase::set_replacement_character(' ');
        else
            // Must set it to something. The first code point is a terrible
            // idea as it's most likely 0, but this is a non-issue for all
            // encodings this code will ever encounter.
            EncoderBase::set_replacement_character(ranges[0].start_code_point);
    }

    void CodePageEncoder::set_replacement_character(char32_t value)
    {
        if (find_char(ranges_, value))
            EncoderBase::set_replacement_character(value);
    }

    size_t CodePageEncoder::get_encoded_size(const char32_t* src, size_t src_size)
    {
        switch (error_policy())
        {
        default:
            break;
        case ErrorPolicy::SKIP:
            return std::count_if(src, src + src_size,
                                 [this](auto c)
                                 {return find_char(ranges_, c).has_value();});
        }
        return src_size;
    }

    std::pair<size_t, size_t>
    CodePageEncoder::encode(const char32_t* src, size_t src_size,
                            void* dst, size_t dst_size)
    {
        auto cdst = static_cast<char*>(dst);
        auto size = std::min(src_size, dst_size);
        for (size_t i = 0; i < size; ++i)
        {
            if (auto c = find_char(ranges_, src[i]))
            {
                *cdst++ = *c;
            }
            else if (error_policy() == ErrorPolicy::REPLACE)
            {
                *cdst++ = *find_char(ranges_, replacement_character());
            }
            else if (error_policy() == ErrorPolicy::THROW)
            {
                throw ConversionException(
                    "Encoding does not support the character.", i);
            }
        }
        return {size, size};
    }

    size_t CodePageEncoder::encode(const char32_t* src, size_t src_size,
                                   std::string& dst)
    {
        for (size_t i = 0; i < src_size; ++i)
        {
            if (auto c = find_char(ranges_, src[i]))
            {
                dst.push_back(*c);
            }
            else if (error_policy() == ErrorPolicy::REPLACE)
            {
                dst.push_back(*find_char(ranges_, replacement_character()));
            }
            else if (error_policy() == ErrorPolicy::THROW)
            {
                throw ConversionException(
                    "Encoding does not support the character.", i);
            }
        }
        return src_size;
    }
}

#endif
