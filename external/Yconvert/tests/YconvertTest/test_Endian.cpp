//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yconvert/Endian.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace Yconvert;

TEST_CASE("system_endianness")
{
    REQUIRE(get_system_endianness() == Endianness::NATIVE);
}

TEST_CASE("reverse_bytes8")
{
    REQUIRE(reverse_bytes<int8_t>(0x10) == 0x10);
}

TEST_CASE("reverse_bytes16")
{
    REQUIRE(reverse_bytes<int16_t>(0x1020) == 0x2010);
}

TEST_CASE("reverse_bytes32")
{
    REQUIRE(reverse_bytes<int32_t>(0x10203040) == 0x40302010);
}

TEST_CASE("reverse_bytes64")
{
    REQUIRE(reverse_bytes(0x1020304050607080LL) == 0x8070605040302010LL);
}
