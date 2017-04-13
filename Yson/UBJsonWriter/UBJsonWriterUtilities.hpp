//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 19.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Common/AssignInteger.hpp"
#include "../UBJsonValueType.hpp"
#include "../YsonException.hpp"
#include "AssignFloat.hpp"
#include "UBJsonValueTraits.hpp"

namespace Yson
{
    #ifdef IS_BIG_ENDIAN

    template <typename T>
    void appendBigEndian(std::vector<char>& buffer, T value)
    {
        auto bytes = reinterpret_cast<char*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(value));
    }

    #else

    template <typename T>
    void appendBigEndian(std::vector<char>& buffer, T value)
    {
        auto first = buffer.size();
        buffer.resize(first + sizeof(value));
        auto bytes = reinterpret_cast<char*>(&value);
        for (unsigned i = 0; i < sizeof(value); ++i)
            buffer[first + i] = bytes[sizeof(value) - i - 1];
    }

    #endif

    void writeMinimalInteger(std::vector<char>& buffer, size_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int16_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int32_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int64_t value);

    template <typename IntT>
    void writeValue(std::vector<char>& buffer, IntT value)
    {
        appendBigEndian(buffer, value);
    }

    template <>
    inline void writeValue(std::vector<char>& buffer, int8_t value)
    {
        buffer.push_back(char(value));
    }

    template <>
    inline void writeValue(std::vector<char>& buffer, uint8_t value)
    {
        buffer.push_back(char(value));
    }

    template <typename IntT>
    void writeValueWithMarker(std::vector<char>& buffer, IntT value)
    {
        buffer.push_back(UBJsonValueTraits<IntT>::marker());
        appendBigEndian(buffer, value);
    }

    template <typename T, typename U>
    void writeIntegerAsImpl(std::vector<char>& buffer, U value)
    {
        T tmp;
        if (assignInteger(tmp, value))
            writeValue(buffer, tmp);
        else
            YSON_THROW("Value is too great or small to be converted: "
                       + std::to_string(value));
    }

    template <typename T>
    void writeIntegerAs(std::vector<char>& buffer, T value,
                        UBJsonValueType valueType)
    {
        switch (valueType)
        {
        case UBJsonValueType::FLOAT_32:
            writeValue(buffer, static_cast<float>(value));
            break;
        case UBJsonValueType::FLOAT_64:
            writeValue(buffer, static_cast<double>(value));
            break;
        case UBJsonValueType::CHAR:
            writeIntegerAsImpl<char>(buffer, value);
            break;
        case UBJsonValueType::UINT_8:
            writeIntegerAsImpl<uint8_t>(buffer, value);
            break;
        case UBJsonValueType::INT_8:
            writeIntegerAsImpl<int8_t>(buffer, value);
            break;
        case UBJsonValueType::INT_16:
            writeIntegerAsImpl<int16_t>(buffer, value);
            break;
        case UBJsonValueType::INT_32:
            writeIntegerAsImpl<int32_t>(buffer, value);
            break;
        case UBJsonValueType::INT_64:
            writeIntegerAsImpl<int64_t>(buffer, value);
            break;
        default:
            YSON_THROW("Can't convert value to " + toString(valueType) + ".");
        }
    }

    template <typename T, typename U>
    void writeFloatAsImpl(std::vector<char>& buffer, U value)
    {
        T tmp;
        if (assignFloat(tmp, value))
            writeValue(buffer, tmp);
        else
            YSON_THROW("Value is too great or small to be converted: "
                       + std::to_string(value));
    }

    template <typename T>
    void writeFloatAs(std::vector<char>& buffer, T value,
                      UBJsonValueType valueType)
    {
        switch (valueType)
        {
        case UBJsonValueType::FLOAT_32:
            writeFloatAsImpl<float>(buffer, value);
            break;
        case UBJsonValueType::FLOAT_64:
            writeFloatAsImpl<double>(buffer, value);
            break;
        default:
            YSON_THROW("Can't convert value to " + toString(valueType) + ".");
        }
    }
}
