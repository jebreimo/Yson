//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 09.04.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include "UBJsonValueType.hpp"

namespace Yson
{
    enum class JsonFormatting
    {
        DEFAULT, NONE, FLAT, FORMAT
    };

    struct JsonParameters
    {
        constexpr JsonParameters() noexcept = default;

        constexpr explicit JsonParameters(JsonFormatting formatting) noexcept
            : formatting(formatting)
        {
        }

        constexpr explicit JsonParameters(size_t valuesPerLine) noexcept
            : valuesPerLine(valuesPerLine),
              formatting(JsonFormatting::FORMAT)
        {
        }

        size_t valuesPerLine = 1;
        JsonFormatting formatting = JsonFormatting::DEFAULT;
    };

    struct UBJsonParameters
    {
        constexpr UBJsonParameters() noexcept = default;

        constexpr explicit UBJsonParameters(ptrdiff_t size) noexcept
            : size(size)
        {
        }

        constexpr UBJsonParameters(ptrdiff_t size,
                                   UBJsonValueType valueType) noexcept
            : size(size), valueType(valueType)
        {
        }

        ptrdiff_t size = -1;
        UBJsonValueType valueType = UBJsonValueType::UNKNOWN;
    };

    struct StructureParameters
    {
        constexpr StructureParameters() noexcept = default;

        constexpr StructureParameters(JsonParameters jsonParameters) noexcept
            : jsonParameters(jsonParameters)
        {
        }

        constexpr StructureParameters(UBJsonParameters parameters) noexcept
            : ubJsonParameters(parameters)
        {
        }

        constexpr StructureParameters(JsonParameters jsonParameters,
                                      UBJsonParameters parameters) noexcept
            : jsonParameters(jsonParameters), ubJsonParameters(parameters)
        {
        }

        JsonParameters jsonParameters;
        UBJsonParameters ubJsonParameters;
    };
}
