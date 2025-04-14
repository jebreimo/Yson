//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <type_traits>

namespace Yconvert
{
    enum class Endianness
    {
        UNKNOWN,
        BIG,
        LITTLE,
        #ifdef YCONVERT_BIG_ENDIAN
        NATIVE = BIG
        #else
        NATIVE = LITTLE
        #endif
    };

    constexpr bool IS_BIG_ENDIAN = Endianness::NATIVE == Endianness::BIG;
    constexpr bool IS_LITTLE_ENDIAN = !IS_BIG_ENDIAN;

    /**
     * @brief Returns the system's endianness detected at runtime.
     *
     * Compared the return value with Endianness::NATIVE to detect
     * inconsistencies.
     */
    [[nodiscard]] inline Endianness get_system_endianness()
    {
        union U {int16_t i16; int8_t i8;} u{int16_t(0x0201)};
        return u.i8 == 0x01 ? Endianness::LITTLE : Endianness::BIG;
    }

    template <typename T, std::enable_if_t<sizeof(T) == 1, int> = 0>
    [[nodiscard]] constexpr T reverse_bytes(T value)
    {
        return value;
    }

    template <typename T, std::enable_if_t<sizeof(T) == 2, int> = 0>
    [[nodiscard]] constexpr T reverse_bytes(T value)
    {
        union U {T v; int8_t b[2];} u {value};
        auto tmp = u.b[0];
        u.b[0] = u.b[1];
        u.b[1] = tmp;
        return u.v;
    }

    template <typename T, std::enable_if_t<sizeof(T) == 4, int> = 0>
    [[nodiscard]] constexpr T reverse_bytes(T value)
    {
        union U
        {
            T v;
            int8_t b[4];
        } u {value};
        int8_t tmp[2] = {u.b[0], u.b[1]};
        u.b[0] = u.b[3];
        u.b[1] = u.b[2];
        u.b[2] = tmp[1];
        u.b[3] = tmp[0];
        return u.v;
    }

    template <typename T, std::enable_if_t<sizeof(T) == 8, int> = 0>
    [[nodiscard]] constexpr T reverse_bytes(T value)
    {
        union U
        {
            T v;
            int8_t b[8];
        } u {value};
        int8_t tmp[4] = {u.b[0], u.b[1], u.b[2], u.b[3]};
        u.b[0] = u.b[7];
        u.b[1] = u.b[6];
        u.b[2] = u.b[5];
        u.b[3] = u.b[4];
        u.b[4] = tmp[3];
        u.b[5] = tmp[2];
        u.b[6] = tmp[1];
        u.b[7] = tmp[0];
        return u.v;
    }

    template <bool SwapBytes, typename T>
    [[nodiscard]] constexpr T swap_endianness(T value)
    {
        if constexpr (SwapBytes)
            return reverse_bytes(value);
        else
            return value;
    }

    template <typename T>
    [[nodiscard]] constexpr T get_big_endian(T value)
    {
        return swap_endianness<IS_LITTLE_ENDIAN>(value);
    }

    template <typename T>
    [[nodiscard]] constexpr T get_little_endian(T value)
    {
        return swap_endianness<IS_BIG_ENDIAN>(value);
    }
}
