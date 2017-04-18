//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>
#include "../YsonDefinitions.hpp"

namespace Yson
{
    enum class JsonTokenType
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
        END_OF_FILE,
        INCOMPLETE_TOKEN,
        /** A line comment, i.e. one that starts with \/\/.
          */
        COMMENT,
        /** @brief A comment that starts with \/\* and ends with *\/ and
          *     can span multiple lines.
          */
        BLOCK_COMMENT,
        WHITESPACE,
        NEWLINE
    };

    std::string toString(JsonTokenType type);

    YSON_API std::ostream& operator<<(std::ostream& stream, JsonTokenType type);
}
