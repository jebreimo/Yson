//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 19.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonWriterUtilities.hpp"
#include <limits>
#include "Yson/DetailedValueType.hpp"

namespace Yson
{
    void writeMinimalInteger(std::vector<char>& buffer, size_t value)
    {
        if (value <= UINT8_MAX)
            writeValueWithMarker(buffer, static_cast<uint8_t>(value));
        else if (value <= INT16_MAX)
            writeValueWithMarker(buffer, static_cast<int16_t>(value));
        else if (value <= INT32_MAX)
            writeValueWithMarker(buffer, static_cast<int32_t>(value));
        else if (value <= INT64_MAX)
            writeValueWithMarker(buffer, static_cast<int64_t>(value));
        else
            YSON_THROW("uint64_t-value is too large: "
                       + std::to_string(value));
    }

    void writeMinimalInteger(std::vector<char>& buffer, int16_t value)
    {
        if (INT8_MIN <= value)
        {
            if (value <= INT8_MAX)
                writeValueWithMarker(buffer, static_cast<int8_t>(value));
            else if (value <= UINT8_MAX)
                writeValueWithMarker(buffer, static_cast<uint8_t>(value));
            else
                writeValueWithMarker(buffer, value);
        }
        else
            writeValueWithMarker(buffer, value);
    }

    void writeMinimalInteger(std::vector<char>& buffer, int32_t value)
    {
        if (INT8_MIN <= value)
        {
            if (value <= INT8_MAX)
                writeValueWithMarker(buffer, static_cast<int8_t>(value));
            else if (value <= UINT8_MAX)
                writeValueWithMarker(buffer, static_cast<uint8_t>(value));
            else if (value <= INT16_MAX)
                writeValueWithMarker(buffer, static_cast<int16_t>(value));
            else
                writeValueWithMarker(buffer, value);
        }
        else if (INT16_MIN <= value)
            writeValueWithMarker(buffer, static_cast<int16_t>(value));
        else
            writeValueWithMarker(buffer, value);
    }

    void writeMinimalInteger(std::vector<char>& buffer, int64_t value)
    {
        if (INT8_MIN <= value)
        {
            if (value <= INT8_MAX)
                writeValueWithMarker(buffer, static_cast<int8_t>(value));
            else if (value <= UINT8_MAX)
                writeValueWithMarker(buffer, static_cast<uint8_t>(value));
            else if (value <= INT16_MAX)
                writeValueWithMarker(buffer, static_cast<int16_t>(value));
            else if (value <= INT32_MAX)
                writeValueWithMarker(buffer, static_cast<int32_t>(value));
            else
                writeValueWithMarker(buffer, value);
        }
        else if (INT16_MIN <= value)
            writeValueWithMarker(buffer, static_cast<int16_t>(value));
        else if (INT32_MIN <= value)
            writeValueWithMarker(buffer, static_cast<int32_t>(value));
        else
            writeValueWithMarker(buffer, value);
    }
}
