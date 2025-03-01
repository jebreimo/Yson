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
    /**
     * @brief Defines the formatting of JSON and UBJSON output.
     *
     * Each array and object can have its own formatting, but the formatting
     * of a child can not be "greater" than the formatting of its parent.
     * For instance, if the parent object has JsonFormatting::FLAT formatting,
     * its children can not have JsonFormatting::FORMAT formatting.
     */
    enum class JsonFormatting
    {
        DEFAULT, ///< Use the same formatting as the parent object or array.
        NONE, ///< No formatting, everything on a single line, no whitespace.
        FLAT, ///< No line breaks, but use spaces between values.
        FORMAT ///< Use line breaks and indentation.
    };

    /**
     * @brief Compares two JsonFormatting values.
     */
    constexpr bool operator<(JsonFormatting a, JsonFormatting b) noexcept
    {
        return static_cast<int>(a) < static_cast<int>(b);
    }

    /**
     * Provides parameters for formatting JSON output.
     */
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

    /**
     * Provides parameters for formatting UBJSON output.
     */
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
