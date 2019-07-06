//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-17.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Ystring {

    struct Endianness
    {
        enum Type
        {
            UNKNOWN,
            BIG,
            LITTLE
        };
    };

    typedef Endianness::Type Endianness_t;

    #ifdef YSTRING_BIG_ENDIAN

    static const bool IsBigEndian = true;

    #else

    static const bool IsBigEndian = false;

    #endif

    static const bool IsLittleEndian = !IsBigEndian;
}
