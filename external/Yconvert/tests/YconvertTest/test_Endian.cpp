//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yconvert/Endian.hpp>
#include <catch2/catch.hpp>

using namespace Yconvert;

TEST_CASE("systemEndianness")
{
    REQUIRE(getSystemEndianness() == Endianness::NATIVE);
}

TEST_CASE("reverseBytes8")
{
    REQUIRE(reverseBytes<int8_t>(0x10) == 0x10);
}

TEST_CASE("reverseBytes16")
{
    REQUIRE(reverseBytes<int16_t>(0x1020) == 0x2010);
}

TEST_CASE("reverseBytes32")
{
    REQUIRE(reverseBytes<int32_t>(0x10203040) == 0x40302010);
}

TEST_CASE("reverseBytes64")
{
    REQUIRE(reverseBytes(0x1020304050607080LL) == 0x8070605040302010LL);
}
