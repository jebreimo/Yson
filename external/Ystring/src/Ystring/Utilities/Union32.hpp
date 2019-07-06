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
    union Union32
    {
        Union32() : u32(0)
        {}

        explicit Union32(char32_t n) : u32(n)
        {}

        template <typename T>
        explicit Union32(const T (&n)[2])
        {
            u16[0] = static_cast<char16_t>(n[0]);
            u16[1] = static_cast<char16_t>(n[1]);
        }

        template <typename T>
        explicit Union32(const T (&n)[4])
        {
            u8[0] = static_cast<uint8_t>(n[0]);
            u8[1] = static_cast<uint8_t>(n[1]);
            u8[2] = static_cast<uint8_t>(n[2]);
            u8[3] = static_cast<uint8_t>(n[3]);
        }

        template <typename T>
        Union32(T a, T b)
        {
            u16[0] = static_cast<char16_t>(a);
            u16[1] = static_cast<char16_t>(b);
        }

        template <typename T>
        Union32(T a, T b, T c, T d)
        {
            u8[0] = static_cast<uint8_t>(a);
            u8[1] = static_cast<uint8_t>(b);
            u8[2] = static_cast<uint8_t>(c);
            u8[3] = static_cast<uint8_t>(d);
        }

        void reverse()
        {
            std::swap(u8[0], u8[3]);
            std::swap(u8[1], u8[2]);
        }

        int32_t i32;
        char32_t u32;
        int16_t i16[2];
        char16_t u16[2];
        int8_t i8[4];
        uint8_t u8[4];
    };
}}
