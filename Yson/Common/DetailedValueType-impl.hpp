//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 09.04.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Yson
{

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

    template <typename T>
    bool isCompatible(DetailedValueType valueType)
    {
        auto type = Details::DetailedValueTraits<T>::valueType();
        return isCompatible(type, valueType);
    }
}
