//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 29.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenType.hpp"
#include <ostream>

namespace Yson
{
    #define CASE_TYPE(value) \
        case JsonTokenType::value: return #value

    std::string toString(JsonTokenType type)
    {
        switch (type)
        {
        CASE_TYPE(INVALID_TOKEN);
        CASE_TYPE(START_ARRAY);
        CASE_TYPE(END_ARRAY);
        CASE_TYPE(START_OBJECT);
        CASE_TYPE(END_OBJECT);
        CASE_TYPE(COLON);
        CASE_TYPE(COMMA);
        CASE_TYPE(STRING);
        CASE_TYPE(VALUE);
        CASE_TYPE(END_OF_FILE);
        CASE_TYPE(INCOMPLETE_TOKEN);
        CASE_TYPE(COMMENT);
        CASE_TYPE(BLOCK_COMMENT);
        CASE_TYPE(WHITESPACE);
        CASE_TYPE(NEWLINE);
        }
        return "<unknown token type: " + std::to_string(int(type)) + ">";
    }

    std::ostream& operator<<(std::ostream& stream, JsonTokenType type)
    {
        return stream << toString(type);
    }
}
