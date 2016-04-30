//****************************************************************************
// Copyright (C) 2011, Jan Erik Breimo.
// Created 2011-07-14 by Jan Erik Breimo
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "EncodeUtf8.hpp"

namespace Ystring { namespace Encodings
{
    size_t encodeUtf8(char (&buffer)[MAX_ENCODED_UTF8_LENGTH], char32_t chr)
    {
        char* begin = buffer;
        char* end = buffer + MAX_ENCODED_UTF8_LENGTH;
        encodeUtf8(begin, end, chr);
        return (size_t)(begin - buffer);
    }

    size_t utf8EncodedLength(char32_t c)
    {
        if (!(c >> 7))
            return 1;
        else if (!(c >> 11))
            return 2;
        else if (!(c >> 16))
            return 3;
        else if (!(c >> 21))
            return 4;
        else if (!(c >> 26))
            return 5;
        else if (!(c >> 31))
            return 6;
        else
            return 7;
    }
}}
