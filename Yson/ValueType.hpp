//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Yson {

    struct ValueType
    {
        enum Enum
        {
//            EMPTY = 'e',
            INTEGER = 'i',
            FLOAT = 'f',
            STRING = 's',
            ARRAY = 'a',
            OBJECT = 'o',
            BIN_INTEGER = '2',
            OCT_INTEGER = '8',
            HEX_INTEGER = 'x',
            TRUE_VALUE = 'T',
            FALSE_VALUE = 'F',
            NULL_VALUE = 'N',
            UNKNOWN = '?',
            INVALID = '0'
        };
    };

    typedef ValueType::Enum ValueType_t;

}
