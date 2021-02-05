//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CodePageEncoder.hpp"

#include <algorithm>
#include <optional>
#include "Yconvert/ConversionException.hpp"

namespace Yconvert
{
    namespace
    {
        std::optional<char>
        findChar(const std::vector<CodePointMapRange>& ranges, char32_t c)
        {
            auto it = std::upper_bound(
                ranges.begin(), ranges.end(),
                CodePointMapRange{c, 0, 0},
                [](auto& a, auto& b){return a.codePoint < b.codePoint;});
            if (it != ranges.begin())
            {
                --it;
                auto offset = c - it->codePoint;
                if (offset <= it->length)
                    return it->index + offset;
            }
            return {};
        }
    }

    CodePageEncoder::CodePageEncoder(Encoding encoding,
                                     const CodePageRange* ranges,
                                     size_t rangesSize)
        : EncoderBase(encoding)
    {
        m_Ranges.reserve(rangesSize);
        char32_t upper = 0;
        for (size_t i = 0; i < rangesSize; ++i)
        {
            if (ranges[i].startIndex != 0xFF || ranges[i].length != 0xFF)
            {
                char32_t cp = upper | ranges[i].startCodePoint;
                m_Ranges.push_back({cp, ranges[i].startIndex, ranges[i].length});
                upper = 0;
            }
            else
            {
                upper = ranges[i].startCodePoint << 16u;
            }
        }
        if (findChar(m_Ranges, REPLACEMENT_CHARACTER))
            EncoderBase::setReplacementCharacter(REPLACEMENT_CHARACTER);
        else if (findChar(m_Ranges, '?'))
            EncoderBase::setReplacementCharacter('?');
        else if (findChar(m_Ranges, ' '))
            EncoderBase::setReplacementCharacter(' ');
        else
            // Must set it to something. The first code point is a terrible
            // idea as it's most likely 0, but this is a non-issue for all
            // encodings this code will ever encounter.
            EncoderBase::setReplacementCharacter(ranges[0].startCodePoint);
    }

    void CodePageEncoder::setReplacementCharacter(char32_t value)
    {
        if (findChar(m_Ranges, value))
            EncoderBase::setReplacementCharacter(value);
    }

    size_t CodePageEncoder::getEncodedSize(const char32_t* src, size_t srcSize)
    {
        switch (errorHandlingPolicy())
        {
        default:
            break;
        case ErrorPolicy::SKIP:
            return std::count_if(src, src + srcSize,
                                 [this](auto c)
                                 {return findChar(m_Ranges, c).has_value();});
        }
        return srcSize;
    }

    std::pair<size_t, size_t>
    CodePageEncoder::encode(const char32_t* src, size_t srcSize,
                            void* dst, size_t dstSize)
    {
        auto cdst = static_cast<char*>(dst);
        auto size = std::min(srcSize, dstSize);
        for (size_t i = 0; i < size; ++i)
        {
            if (auto c = findChar(m_Ranges, src[i]))
            {
                *cdst++ = *c;
            }
            else if (errorHandlingPolicy() == ErrorPolicy::REPLACE)
            {
                *cdst++ = *findChar(m_Ranges, replacementCharacter());
            }
            else if (errorHandlingPolicy() == ErrorPolicy::THROW)
            {
                throw ConversionException(
                    "Encoding does not support the character.", i);
            }
        }
        return {size, size};
    }

    size_t CodePageEncoder::encode(const char32_t* src, size_t srcSize,
                                   std::string& dst)
    {
        for (size_t i = 0; i < srcSize; ++i)
        {
            if (auto c = findChar(m_Ranges, src[i]))
            {
                dst.push_back(*c);
            }
            else if (errorHandlingPolicy() == ErrorPolicy::REPLACE)
            {
                dst.push_back(*findChar(m_Ranges, replacementCharacter()));
            }
            else if (errorHandlingPolicy() == ErrorPolicy::THROW)
            {
                throw ConversionException(
                    "Encoding does not support the character.", i);
            }
        }
        return srcSize;
    }
}
