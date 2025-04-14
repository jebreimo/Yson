//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonTokenType.hpp"
#include <ostream>
#include <string>

namespace Yson
{
    namespace
    {
        const char LegalTypes[] =
                "CD\0F\0HI\0\0L\0N\0\0\0\0STU\0\0\0\0Z[\0]\0\0\0\0\0\0d\0\0"
                "\0\0i\0\0l\0\0\0\0\0\0\0\0\0\0\0\0\0\0{\0}";
    }
    #define CASE_TYPE(value) \
        case UBJsonTokenType::value: return #value

    std::string toString(UBJsonTokenType type)
    {
        switch (type)
        {
        CASE_TYPE(NULL_TOKEN);
        CASE_TYPE(NO_OP_TOKEN);
        CASE_TYPE(TRUE_TOKEN);
        CASE_TYPE(FALSE_TOKEN);
        CASE_TYPE(INT8_TOKEN);
        CASE_TYPE(UINT8_TOKEN);
        CASE_TYPE(INT16_TOKEN);
        CASE_TYPE(INT32_TOKEN);
        CASE_TYPE(INT64_TOKEN);
        CASE_TYPE(FLOAT32_TOKEN);
        CASE_TYPE(FLOAT64_TOKEN);
        CASE_TYPE(HIGH_PRECISION_TOKEN);
        CASE_TYPE(CHAR_TOKEN);
        CASE_TYPE(STRING_TOKEN);
        CASE_TYPE(START_OBJECT_TOKEN);
        CASE_TYPE(END_OBJECT_TOKEN);
        CASE_TYPE(START_ARRAY_TOKEN);
        CASE_TYPE(END_ARRAY_TOKEN);
        CASE_TYPE(START_OPTIMIZED_OBJECT_TOKEN);
        CASE_TYPE(START_OPTIMIZED_ARRAY_TOKEN);
        default:
            break;
        }
        return "<unknown value type: " + std::to_string(int(type)) + ">";
    }

    std::ostream& operator<<(std::ostream& stream, UBJsonTokenType type)
    {
        return stream << toString(type);
    }

    bool isValidTokenType(char c)
    {
        c -= 'C';
        return 0 < c && c < sizeof(LegalTypes) && LegalTypes[c] != 0;
    }
}
