//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 21.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include "Yson/UBJsonValueType.hpp"

namespace Yson
{
    template <typename T>
    struct UBJsonValueTraits
    {
    };

    #define YSON_DEFINE_UBJSONVALUE_TRAITS(cppType, ubjsonType) \
        template <> \
        struct UBJsonValueTraits<cppType> \
        { \
            constexpr static UBJsonValueType type() {return ubjsonType;} \
            constexpr static char marker() {return char(ubjsonType);} \
        }

    YSON_DEFINE_UBJSONVALUE_TRAITS(int8_t, UBJsonValueType::INT_8);
    YSON_DEFINE_UBJSONVALUE_TRAITS(uint8_t, UBJsonValueType::UINT_8);
    YSON_DEFINE_UBJSONVALUE_TRAITS(int16_t, UBJsonValueType::INT_16);
    YSON_DEFINE_UBJSONVALUE_TRAITS(int32_t, UBJsonValueType::INT_32);
    YSON_DEFINE_UBJSONVALUE_TRAITS(int64_t, UBJsonValueType::INT_64);
    YSON_DEFINE_UBJSONVALUE_TRAITS(float, UBJsonValueType::FLOAT_32);
    YSON_DEFINE_UBJSONVALUE_TRAITS(double, UBJsonValueType::FLOAT_64);
    YSON_DEFINE_UBJSONVALUE_TRAITS(char, UBJsonValueType::CHAR);
}
