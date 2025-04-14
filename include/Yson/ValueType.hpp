//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>
#include "YsonDefinitions.hpp"

namespace Yson
{
    enum class ValueType
    {
        UNKNOWN,
        NULL_VALUE,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        ARRAY,
        OBJECT,
        INVALID
    };

    YSON_API std::string toString(ValueType type);

    YSON_API std::ostream& operator<<(std::ostream& stream, ValueType type);
}
