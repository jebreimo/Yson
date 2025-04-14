//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/DetailedValueType.hpp"
#include <ostream>

namespace Yson
{
    namespace
    {
        uint8_t getFlags(DetailedValueType valueType);
    }

    #define CASE_TYPE(value) \
        case DetailedValueType::value: return #value

    std::string toString(DetailedValueType type)
    {
        switch (type)
        {
        CASE_TYPE(UNKNOWN);
        CASE_TYPE(INVALID);
        CASE_TYPE(NULL_VALUE);
        CASE_TYPE(BOOLEAN);
        CASE_TYPE(CHAR);
        CASE_TYPE(STRING);
        CASE_TYPE(OBJECT);
        CASE_TYPE(ARRAY);
        CASE_TYPE(UINT_7);
        CASE_TYPE(UINT_8);
        CASE_TYPE(SINT_8);
        CASE_TYPE(UINT_15);
        CASE_TYPE(UINT_16);
        CASE_TYPE(SINT_16);
        CASE_TYPE(UINT_31);
        CASE_TYPE(UINT_32);
        CASE_TYPE(SINT_32);
        CASE_TYPE(UINT_63);
        CASE_TYPE(UINT_64);
        CASE_TYPE(SINT_64);
        CASE_TYPE(FLOAT_32);
        CASE_TYPE(FLOAT_64);
        CASE_TYPE(BIG_INT);
        CASE_TYPE(HIGH_PRECISION_NUMBER);
        default:
            break;
        }
        return "<unknown detailed value type: " + std::to_string(int(type))
               + ">";
    }

    std::ostream& operator<<(std::ostream& stream, DetailedValueType type)
    {
        return stream << toString(type);
    }

    bool isCompatible(DetailedValueType type, DetailedValueType queryType)
    {
        auto typeFlags = getFlags(type);
        return (typeFlags & getFlags(queryType)) == typeFlags;
    }

    namespace
    {
        uint8_t getFlags(DetailedValueType valueType)
        {
            switch (valueType)
            {
            case DetailedValueType::STRING:
                return 0x40;
            case DetailedValueType::CHAR:
            case DetailedValueType::UINT_7:
                return 0x7F;
            case DetailedValueType::UINT_8:
                return 0x7E;
            case DetailedValueType::SINT_8:
                return 0x7D;
            case DetailedValueType::UINT_15:
                return 0x7B;
            case DetailedValueType::UINT_16:
                return 0x7A;
            case DetailedValueType::SINT_16:
                return 0x79;
            case DetailedValueType::UINT_31:
                return 0x73;
            case DetailedValueType::UINT_32:
                return 0x72;
            case DetailedValueType::SINT_32:
                return 0x71;
            case DetailedValueType::UINT_63:
                return 0x63;
            case DetailedValueType::UINT_64:
                return 0x62;
            case DetailedValueType::SINT_64:
                return 0x61;
            case DetailedValueType::FLOAT_32:
            case DetailedValueType::FLOAT_64:
                return 0x60;
            case DetailedValueType::BIG_INT:
                return 0x43;
            default:
                return 0xFF;
            }
        }
    }
}
