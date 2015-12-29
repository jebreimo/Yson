//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 11.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Yson
{
    struct JsonTokenType
    {
        enum Type
        {
            INVALID_TOKEN,
            START_ARRAY,
            END_ARRAY,
            START_OBJECT,
            END_OBJECT,
            COLON,
            COMMA,
            STRING,
            VALUE,
            END_OF_BUFFER,
            BLOCK_STRING,
            /** A line comment, i.e. one that starts with //.
              */
            COMMENT,
            /** @brief A comment that starts with \/\* and ends with *\/ and
              *     can span multiple lines.
              */
            BLOCK_COMMENT,
            WHITESPACE,
            NEWLINE
        };
    };

    typedef JsonTokenType::Type JsonTokenType_t;
}
