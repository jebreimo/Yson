//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf32Encoder.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace Yconvert;

TEST_CASE("Test Utf32LEEncoder")
{
    char32_t chars[2] = {U'√', U'Ω'};
    Utf32LEEncoder encoder;
    std::string s;
    REQUIRE(encoder.encode(chars, 2, s) == 2);
    REQUIRE_THAT(s, Catch::Matchers::Equals(std::string("\x1A\x22\x00\x00\xA9\x03\x00\x00", 8)));

    char t[8] = {};
    REQUIRE(encoder.encode(chars, 2, t, 7) == std::pair(size_t(1), size_t(4)));
    REQUIRE(t[0] == 0x1A);
    REQUIRE(t[1] == 0x22);
    REQUIRE(t[2] == 0x00);
    REQUIRE(t[3] == 0x00);

    REQUIRE(encoder.encode(chars, 2, t, 8) == std::pair(size_t(2), size_t(8)));
    REQUIRE(t[0] == 0x1A);
    REQUIRE(t[1] == 0x22);
    REQUIRE(t[2] == 0x00);
    REQUIRE(t[3] == 0x00);
    REQUIRE(t[4] == -0x57);
    REQUIRE(t[5] == 0x03);
    REQUIRE(t[6] == 0x00);
    REQUIRE(t[7] == 0x00);
}

TEST_CASE("Test Utf32BEEncoder")
{
    char32_t chars[2] = {0x221A, 0x3A9};
    Utf32BEEncoder encoder;
    std::string s;
    REQUIRE(encoder.encode(chars, 2, s) == 2);
    REQUIRE_THAT(s, Catch::Matchers::Equals(std::string("\x00\x00\x22\x1A\x00\x00\x03\xA9", 8)));
    char t[7] = {};
    REQUIRE(encoder.encode(chars, 2, t, 7) == std::pair(size_t(1), size_t(4)));
    REQUIRE(t[0] == 0x00);
    REQUIRE(t[1] == 0x00);
    REQUIRE(t[2] == 0x22);
    REQUIRE(t[3] == 0x1A);
}
