//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../ValueType.hpp"
#include <ostream>
#include <string>

namespace Yson
{
    #define CASE_TYPE(value) \
        case ValueType::value: return #value

    std::string toString(ValueType type)
    {
        switch (type)
        {
        CASE_TYPE(UNKNOWN);
        CASE_TYPE(NULL_VALUE);
        CASE_TYPE(BOOLEAN);
        CASE_TYPE(INTEGER);
        CASE_TYPE(FLOAT);
        CASE_TYPE(STRING);
        CASE_TYPE(ARRAY);
        CASE_TYPE(OBJECT);
        CASE_TYPE(INVALID);
        default: break;
        }
        return "<unknown value type: " + std::to_string(int(type)) + ">";
    }

    std::ostream& operator<<(std::ostream& stream, ValueType type)
    {
        return stream << toString(type);
    }
}
