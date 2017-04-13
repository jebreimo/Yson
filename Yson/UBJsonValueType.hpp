//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include "YsonDefinitions.hpp"

namespace Yson
{
    enum class UBJsonValueType : char
    {
        UNKNOWN = '\0',
        NULL_VALUE = 'Z',
        TRUE_VALUE = 'T',
        FALSE_VALUE = 'F',
        FLOAT_32 = 'd',
        FLOAT_64 = 'D',
        CHAR = 'C',
        STRING = 'S',
        HIGH_PRECISION_NUMBER = 'H',
        OBJECT = '{',
        ARRAY = '[',
        UINT_8 = 'U',
        INT_8 = 'i',
        INT_16 = 'I',
        INT_32 = 'l',
        INT_64 = 'L'
    };

    YSON_API std::string toString(UBJsonValueType type);

    YSON_API std::ostream& operator<<(std::ostream& stream,
                                      UBJsonValueType type);
}
