//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/UBJsonValueType.hpp"
#include <ostream>

namespace Yson
{
    #define CASE_TYPE(value) \
        case UBJsonValueType::value: return #value

    std::string toString(UBJsonValueType type)
    {
        switch (type)
        {
        CASE_TYPE(UNKNOWN);
        CASE_TYPE(NULL_VALUE);
        CASE_TYPE(TRUE_VALUE);
        CASE_TYPE(FALSE_VALUE);
        CASE_TYPE(FLOAT_32);
        CASE_TYPE(FLOAT_64);
        CASE_TYPE(CHAR);
        CASE_TYPE(STRING);
        CASE_TYPE(HIGH_PRECISION_NUMBER);
        CASE_TYPE(OBJECT);
        CASE_TYPE(ARRAY);
        CASE_TYPE(UINT_8);
        CASE_TYPE(INT_8);
        CASE_TYPE(INT_16);
        CASE_TYPE(INT_32);
        CASE_TYPE(INT_64);
        default:
            break;
        }
        return "<unknown UBJsonValueType: " + std::to_string(int(type))
               + ">";
    }

    std::ostream& operator<<(std::ostream& stream, UBJsonValueType type)
    {
        return stream << toString(type);
    }
}
