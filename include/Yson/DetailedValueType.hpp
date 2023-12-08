//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <iosfwd>
#include <string>
#include "YsonDefinitions.hpp"

namespace Yson
{
    enum class DetailedValueType
    {
        UNKNOWN,
        INVALID,
        NULL_VALUE,
        BOOLEAN,
        STRING,
        OBJECT,
        ARRAY,
        CHAR,
        UINT_7,
        UINT_8,
        SINT_8,
        UINT_15,
        UINT_16,
        SINT_16,
        UINT_31,
        UINT_32,
        SINT_32,
        UINT_63,
        UINT_64,
        SINT_64,
        FLOAT_32,
        FLOAT_64,
        BIG_INT,
        HIGH_PRECISION_NUMBER
    };

    YSON_API std::string toString(DetailedValueType type);

    YSON_API std::ostream& operator<<(std::ostream& stream,
                                      DetailedValueType type);

    /** @brief Returns true if a value identified as @a type can be
      *     assigned to a variable of type @a compatType.
      * @note This function is intended to be used to test whether an integer
      *     value in a JSON document will fit in an integer variable of a
      *     certain size without overflow.
      */
    YSON_API bool isCompatible(DetailedValueType compatType,
                               DetailedValueType type);

    namespace Details
    {
        template <typename T>
        struct DetailedValueTraits
        {
            constexpr static DetailedValueType valueType()
            {
                return DetailedValueType::UNKNOWN;
            }
        };

        #define YSON_DETAIL_DETAILEDVALUETRAITS(type, detValueType) \
            template <> \
            struct DetailedValueTraits<type> \
            { \
                constexpr static DetailedValueType valueType() \
                {return detValueType;} \
            }

        YSON_DETAIL_DETAILEDVALUETRAITS(char, DetailedValueType::CHAR);
        YSON_DETAIL_DETAILEDVALUETRAITS(int8_t, DetailedValueType::SINT_8);
        YSON_DETAIL_DETAILEDVALUETRAITS(uint8_t, DetailedValueType::UINT_8);
        YSON_DETAIL_DETAILEDVALUETRAITS(int16_t, DetailedValueType::SINT_16);
        YSON_DETAIL_DETAILEDVALUETRAITS(uint16_t, DetailedValueType::UINT_16);
        YSON_DETAIL_DETAILEDVALUETRAITS(int32_t, DetailedValueType::SINT_32);
        YSON_DETAIL_DETAILEDVALUETRAITS(uint32_t, DetailedValueType::UINT_32);
        YSON_DETAIL_DETAILEDVALUETRAITS(int64_t, DetailedValueType::SINT_64);
        YSON_DETAIL_DETAILEDVALUETRAITS(uint64_t, DetailedValueType::UINT_64);
        YSON_DETAIL_DETAILEDVALUETRAITS(float, DetailedValueType::FLOAT_32);
        YSON_DETAIL_DETAILEDVALUETRAITS(double, DetailedValueType::FLOAT_64);
        YSON_DETAIL_DETAILEDVALUETRAITS(std::string, DetailedValueType::STRING);
    }

    /** @brief Returns true if a value of type T can hold a value of type
      *     @a valueType.
      */
    template <typename T>
    bool isCompatible(DetailedValueType valueType)
    {
        auto type = Details::DetailedValueTraits<T>::valueType();
        return isCompatible(type, valueType);
    }
}
