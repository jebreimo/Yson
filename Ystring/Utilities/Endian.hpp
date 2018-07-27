//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <type_traits>
#include "../Endianness.hpp"
#include "../PlatformDetails.hpp"
#include "Union16.hpp"
#include "Union32.hpp"

namespace Ystring { namespace Utilities
{
    /** @brief Returns the system's endianness.
     */
    inline Endianness_t systemEndianness()
    {
        Union16 u(1);
        return u.u8[0] == 0 ? Endianness::BIG
                            : Endianness::LITTLE;
    }

    inline uint8_t reverseBytes(uint8_t v)
    {
        return v;
    }

    inline int8_t reverseBytes(int8_t v)
    {
        return v;
    }

    inline char reverseBytes(char v)
    {
        return v;
    }

    inline char16_t reverseBytes(char16_t v)
    {
        Union16 u(v);
        u.reverse();
        return u.u16;
    }

    inline int16_t reverseBytes(int16_t v)
    {
        return (int16_t)reverseBytes((char16_t)v);
    }

    inline char32_t reverseBytes(char32_t v)
    {
        Union32 u(v);
        u.reverse();
        return u.u32;
    }

    inline int32_t reverseBytes(int32_t v)
    {
        return (int32_t)reverseBytes((char32_t)v);
    }

    inline wchar_t reverseBytes(wchar_t v)
    {
        auto alias = reinterpret_cast<uint8_t*>(v);
        for (int i = 0; i < sizeof(v) / 2; ++i)
            alias[i] = alias[sizeof(v) - i - 1];
        return v;
    }

    inline Union16 reverseBytes(Union16 value)
    {
        value.reverse();
        return value;
    }

    inline Union32 reverseBytes(Union32 value)
    {
        value.reverse();
        return value;
    }

    template <bool SwapBytes, typename T>
    struct EndianSwapper
    {
    };

    template <typename T>
    struct EndianSwapper<true, T>
    {
        static void swap(T& value)
        {
            value = reverseBytes(value);
        }
    };

    template <typename T>
    struct EndianSwapper<false, T>
    {
        static void swap(T& value)
        {}
    };

    template <bool SwapBytes, typename T>
    void swapEndianness(T& value)
    {
        EndianSwapper<SwapBytes, T>::swap(value);
    }

    template <typename T>
    T fromBigEndian(T value)
    {
        EndianSwapper<IsLittleEndian, T>::swap(value);
        return value;
    }

    template <typename T>
    T fromLittleEndian(T value)
    {
        EndianSwapper<IsBigEndian, T>::swap(value);
        return value;
    }

    inline int16_t bigI16(int8_t a, int8_t b)
    {
        Union16 u(a, b);
        swapEndianness<IsBigEndian>(u);
        return u.i16;
    }

    inline char16_t bigU16(uint8_t a, uint8_t b)
    {
        Union16 u(a, b);
        swapEndianness<IsBigEndian>(u);
        return u.u16;
    }

    inline char32_t bigI32(int8_t a, int8_t b, int8_t c, int8_t d)
    {
        Union32 u(a, b, c, d);
        swapEndianness<IsBigEndian>(u);
        return u.i32;
    }

    inline char32_t bigU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
        Union32 u(a, b, c, d);
        swapEndianness<IsBigEndian>(u);
        return u.u32;
    }

    inline int16_t littleI16(int8_t a, int8_t b)
    {
        Union16 u(b, a);
        swapEndianness<IsBigEndian>(u);
        return u.i16;
    }

    inline char16_t littleU16(uint8_t a, uint8_t b)
    {
        Union16 u(b, a);
        swapEndianness<IsBigEndian>(u);
        return u.u16;
    }

    inline char32_t littleI32(int8_t a, int8_t b, int8_t c, int8_t d)
    {
        Union32 u(d, c, b, a);
        swapEndianness<IsBigEndian>(u);
        return u.i32;
    }

    inline char32_t littleU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
        Union32 u(d, c, b, a);
        swapEndianness<IsBigEndian>(u);
        return u.u32;
    }
}}
