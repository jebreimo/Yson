//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf16Encoder.hpp"
#include <catch2/catch.hpp>

using namespace Yconvert;

void checkAddBytesLE(char32_t c, std::string_view expected)
{
    std::string s;
    Utf16LEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, s) == 1);
    REQUIRE(s == expected);
}

void checkAddBytesBE(char32_t c, std::string_view expected)
{
    std::string s;
    Utf16BEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, s) == 1);
    REQUIRE(s == expected);
}

TEST_CASE("Test add UTF-16 LE bytes.")
{
    checkAddBytesLE(U'', {"\xFF\xF8", 2});
}

TEST_CASE("Test add UTF-16 BE bytes.")
{
    checkAddBytesBE(U'', {"\xF8\xFF", 2});
}

void checkEncodeBytesLE(char32_t c, std::string_view expected)
{
    char str[4] = {};
    Utf16LEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, str, 4) == std::pair(size_t(1), expected.size()));
    for (size_t i = 0; i < expected.size(); ++i)
        REQUIRE(str[i] == expected[i]);
}

void checkEncodeBytesBE(char32_t c, std::string_view expected)
{
    char str[4] = {};
    Utf16BEEncoder encoder;
    REQUIRE(encoder.encode(&c, 1, str, 4) == std::pair(size_t(1), expected.size()));
    for (size_t i = 0; i < expected.size(); ++i)
        REQUIRE(str[i] == expected[i]);
}

TEST_CASE("Test encode ASCII")
{
    checkEncodeBytesLE(U'A', {"A\0", 2});
    checkEncodeBytesBE(U'A', {"\0A", 2});
}
