//****************************************************************************
// Copyright © 2011, Jan Erik Breimo.
// Created 2011-07-14 by Jan Erik Breimo
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <algorithm>

namespace Ystring { namespace Encodings
{
    namespace Detail
    {
        template <typename OutputIt>
        OutputIt addUtf8(OutputIt it, char32_t c, size_t length)
        {
            if (length == 1)
            {
                *it = (char)c;
                ++it;
            }
            else
            {
                auto shift = (length - 1) * 6;
                *it = (char)((0xFF << (8 - length)) | (c >> shift));
                ++it;
                for (size_t i = 1; i < length; i++)
                {
                    shift -= 6;
                    *it = (char)(0x80 | ((c >> shift) & 0x3F));
                    ++it;
                }
            }
            return it;
        }
    }

    template <typename OutputIt>
    OutputIt addUtf8(OutputIt it, char32_t c)
    {
        return Detail::addUtf8(it, c, utf8EncodedLength(c));
    }

    template <typename RndIt>
    size_t encodeUtf8(RndIt& begin, RndIt end, char32_t c)
    {
        size_t length = utf8EncodedLength(c);
        if (length > (size_t)std::distance(begin, end))
            return 0;
        begin = Detail::addUtf8(begin, c, length);
        return length;
    }
}}
