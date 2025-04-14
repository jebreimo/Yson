//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>
#include "Yson/YsonDefinitions.hpp"

namespace Yson
{
    enum class UBJsonTokenType : char
    {
        UNKNOWN_TOKEN = '\0',
        NULL_TOKEN = 'Z',
        NO_OP_TOKEN = 'N',
        TRUE_TOKEN = 'T',
        FALSE_TOKEN = 'F',
        INT8_TOKEN = 'i',
        UINT8_TOKEN = 'U',
        INT16_TOKEN = 'I',
        INT32_TOKEN = 'l',
        INT64_TOKEN = 'L',
        FLOAT32_TOKEN = 'd',
        FLOAT64_TOKEN = 'D',
        HIGH_PRECISION_TOKEN = 'H',
        CHAR_TOKEN = 'C',
        STRING_TOKEN = 'S',
        START_OBJECT_TOKEN = '{',
        END_OBJECT_TOKEN = '}',
        START_ARRAY_TOKEN = '[',
        END_ARRAY_TOKEN = ']',
        START_OPTIMIZED_OBJECT_TOKEN = '\1',
        START_OPTIMIZED_ARRAY_TOKEN = '\2',
        OBJECT_KEY_TOKEN = '\3'
    };

    std::string toString(UBJsonTokenType type);

    YSON_API std::ostream& operator<<(std::ostream& stream, UBJsonTokenType type);

    bool isValidTokenType(char c);
}
