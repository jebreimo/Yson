//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <algorithm>
#include <cstdint>

namespace Ystring { namespace Utilities
{
    union Union16
    {
        Union16() : u16(0)
        {}

        explicit Union16(char16_t n) : u16(n)
        {}

        template <typename T>
        explicit Union16(const T (&n)[2])
        {
            u8[0] = static_cast<uint8_t>(n[0]);
            u8[1] = static_cast<uint8_t>(n[1]);
        }

        template <typename T>
        Union16(T a, T b)
        {
            u8[0] = static_cast<uint8_t>(a);
            u8[1] = static_cast<uint8_t>(b);
        }

        void reverse()
        {
            std::swap(u8[0], u8[1]);
        }

        char16_t u16;
        int16_t i16;
        uint8_t u8[2];
        int8_t i8[2];
    };
}}
