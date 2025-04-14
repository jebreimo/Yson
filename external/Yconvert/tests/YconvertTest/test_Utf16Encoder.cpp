//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf16Encoder.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Yconvert;

void check_add_bytes_le(char32_t c, std::string_view expected)
{
    std::string s;
    Utf16LEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, s) == 1);
    REQUIRE(s == expected);
}

void check_add_bytes_be(char32_t c, std::string_view expected)
{
    std::string s;
    Utf16BEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, s) == 1);
    REQUIRE(s == expected);
}

TEST_CASE("Test add UTF-16 LE bytes.")
{
    check_add_bytes_le(U'', {"\xFF\xF8", 2});
}

TEST_CASE("Test add UTF-16 BE bytes.")
{
    check_add_bytes_be(U'', {"\xF8\xFF", 2});
}

void check_encode_bytes_le(char32_t c, std::string_view expected)
{
    char str[4] = {};
    Utf16LEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, str, 4) == std::pair(size_t(1), expected.size()));
    for (size_t i = 0; i < expected.size(); ++i)
        REQUIRE(str[i] == expected[i]);
}

void check_encode_bytes_be(char32_t c, std::string_view expected)
{
    char str[4] = {};
    Utf16BEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, str, 4) == std::pair(size_t(1), expected.size()));
    for (size_t i = 0; i < expected.size(); ++i)
        REQUIRE(str[i] == expected[i]);
}

TEST_CASE("Test encode ASCII")
{
    check_encode_bytes_le(U'A', {"A\0", 2});
    check_encode_bytes_be(U'A', {"\0A", 2});
}
