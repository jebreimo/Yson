//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 01.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include "../Common/ThrowYsonException.hpp"
#include "FromBigEndian.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    template <typename T, typename U,
              typename std::enable_if<sizeof(U) == 1, int>::type = 0>
    T convertIntegerImpl(const void* value)
    {
        return static_cast<T>(*static_cast<const U*>(value));
    }

    template <typename T, typename U,
              typename std::enable_if<sizeof(U) != 1, int>::type = 0>
    T convertIntegerImpl(const void* value)
    {
        U tmp = *static_cast<const U*>(value);
        U result;
        fromBigEndian<sizeof(U)>(&result, &tmp);
        return static_cast<T>(result);
    }

    template <typename T>
    T convertInteger(UBJsonTokenType type, const void* value)
    {
        switch (type)
        {
        case UBJsonTokenType::NULL_TOKEN:
        case UBJsonTokenType::FALSE_TOKEN:
            return 0;
        case UBJsonTokenType::TRUE_TOKEN:
            return 1;
        case UBJsonTokenType::INT8_TOKEN:
            return convertIntegerImpl<T, int8_t>(value);
        case UBJsonTokenType::UINT8_TOKEN:
            return convertIntegerImpl<T, uint8_t>(value);
        case UBJsonTokenType::INT16_TOKEN:
            return convertIntegerImpl<T, int16_t>(value);
        case UBJsonTokenType::INT32_TOKEN:
            return convertIntegerImpl<T, int32_t>(value);
        case UBJsonTokenType::INT64_TOKEN:
            return convertIntegerImpl<T, int64_t>(value);
        case UBJsonTokenType::CHAR_TOKEN:
            return convertIntegerImpl<T, int64_t>(value);
        default:
            break;
        }
        YSON_THROW("Can't convert " + toString(type) + " to integer.");
    }

}
