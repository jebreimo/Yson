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
#include "Yson/UBJsonValueType.hpp"
#include "Yson/Common/AssignInteger.hpp"
#include "Yson/Common/ThrowYsonException.hpp"
#include "AssignFloat.hpp"
#include "UBJsonValueTraits.hpp"

namespace Yson
{
    #ifdef IS_BIG_ENDIAN

    template <size_t N>
    void appendBigEndian(std::vector<char>& buffer, const void* value)
    {
        auto bytes = static_cast<const char*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + N);
    }

    #else

    template <size_t N>
    void appendBigEndian(std::vector<char>& buffer, const void* value)
    {
        auto first = buffer.size();
        buffer.resize(first + N);
        for (unsigned i = 0; i < N; ++i)
            buffer[first + i] = static_cast<const char*>(value)[N - i - 1];
    }

    template <>
    inline void appendBigEndian<1>(std::vector<char>& buffer, const void* value)
    {
        buffer.push_back(static_cast<const char*>(value)[0]);
    }

    template <>
    inline void appendBigEndian<2>(std::vector<char>& buffer, const void* value)
    {
        auto first = buffer.size();
        buffer.resize(first + 2);
        auto dst = buffer.data() + first;
        auto src = static_cast<const char*>(value);
        dst[0] = src[1];
        dst[1] = src[0];
    }

    template <>
    inline void appendBigEndian<4>(std::vector<char>& buffer, const void* value)
    {
        auto first = buffer.size();
        buffer.resize(first + 4);
        auto dst = buffer.data() + first;
        auto src = static_cast<const char*>(value);
        dst[0] = src[3];
        dst[1] = src[2];
        dst[2] = src[1];
        dst[3] = src[0];
    }

    template <>
    inline void appendBigEndian<8>(std::vector<char>& buffer, const void* value)
    {
        auto first = buffer.size();
        buffer.resize(first + 8);
        auto dst = buffer.data() + first;
        auto src = static_cast<const char*>(value);
        dst[0] = src[7];
        dst[1] = src[6];
        dst[2] = src[5];
        dst[3] = src[4];
        dst[4] = src[3];
        dst[5] = src[2];
        dst[6] = src[1];
        dst[7] = src[0];
    }

    #endif

    void writeMinimalInteger(std::vector<char>& buffer, size_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int16_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int32_t value);

    void writeMinimalInteger(std::vector<char>& buffer, int64_t value);

    template <typename IntT>
    void writeValue(std::vector<char>& buffer, IntT value)
    {
        appendBigEndian<sizeof(IntT)>(buffer, &value);
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
        writeValue(buffer, value);
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
