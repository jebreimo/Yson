//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 09.04.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "UBJsonValueType.hpp"

namespace Yson
{
    enum class JsonFormatting
    {
        DEFAULT, NONE, FLAT, FORMAT
    };

    struct JsonParameters
    {
        JsonParameters()
        {}

        JsonParameters(JsonFormatting formatting)
                : formatting(formatting)
        {}

        JsonParameters(size_t valuesPerLine)
                : valuesPerLine(valuesPerLine),
                  formatting(JsonFormatting::FORMAT)
        {}

        size_t valuesPerLine = 1;
        JsonFormatting formatting = JsonFormatting::DEFAULT;
    };

    struct UBJsonParameters
    {
        UBJsonParameters()
        {}

        explicit UBJsonParameters(ptrdiff_t size)
                : size(size)
        {}

        UBJsonParameters(ptrdiff_t size, UBJsonValueType valueType)
                : size(size), valueType(valueType)
        {}

        ptrdiff_t size = -1;
        UBJsonValueType valueType = UBJsonValueType::UNKNOWN;
    };

    struct StructureParameters
    {
        StructureParameters()
        {}

        StructureParameters(JsonParameters jsonParameters)
                : jsonParameters(jsonParameters)
        {}

        StructureParameters(UBJsonParameters parameters)
                : ubJsonParameters(parameters)
        {}

        StructureParameters(JsonParameters jsonParameters,
                            UBJsonParameters parameters)
                : ubJsonParameters(parameters), jsonParameters(jsonParameters)
        {}

        JsonParameters jsonParameters;
        UBJsonParameters ubJsonParameters;
    };
}
