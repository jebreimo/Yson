//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <ostream>

namespace Yson
{
    enum class ValueType : char
    {
        /** @brief An integer value.
          */
        INTEGER = 'i',

        /** @brief A floating point value.
          */
        FLOAT = 'f',

        /** @brief A string value.
          */
        STRING = 's',

        /** @brief An array.
          */
        ARRAY = 'a',

        /** @brief An object.
          */
        OBJECT = 'o',

        /** @brief A binary integer value.
          *
          * @note This type is only used internally.
          */
        BIN_INTEGER = '2',

        /** @brief An octal integer value.
          *
          * @note This type is only used internally.
          */
        OCT_INTEGER = '8',

        /** @brief An hexadecimal integer value.
          *
          * @note This type is only used internally.
          */
        HEX_INTEGER = 'x',

        /** @brief The boolean value "true".
          */
        TRUE_VALUE = 'T',

        /** @brief The boolean value "false".
          */
        FALSE_VALUE = 'F',

        /** @brief The value "null".
          */
        NULL_VALUE = 'N',

        /** @brief One of the floating point values not supported by JSON.
          *
          * Four values are possible: NaN, infinity, +infinity
          * and -infinty.
          * @note This type is only used internally.
          */
        EXTENDED_FLOAT = '#',

        /** @brief An unrecognized value.
          *
          * @note This type is only used internally.
          */
        UNKNOWN = '?',

        /** @brief An invalid value.
          *
          * @note This type is only used internally.
          */
        INVALID = '0'
    };

    template <typename C, typename T>
    std::basic_ostream<C, T>& operator<<(std::basic_ostream<C, T>& stream,
                                         ValueType valueType)
    {
        return stream << char(valueType);
    }
}
