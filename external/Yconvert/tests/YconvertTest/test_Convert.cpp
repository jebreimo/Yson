//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

#include <catch2/catch.hpp>

using namespace Yconvert;

TEST_CASE("Convert ISO 8859-10 -> UTF-8")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convertTo<std::string>(s, Encoding::ISO_8859_10, Encoding::UTF_8) == u8"ABįņ");
}

TEST_CASE("Convert ISO 8859-10 -> UTF-16")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convertTo<std::u16string>(s, Encoding::ISO_8859_10, Encoding::UTF_16_NATIVE) == u"ABįņ");
}

TEST_CASE("Convert UTF-32 -> UTF-16")
{
    std::u32string s = U"ABįņ";
    REQUIRE(
        convertTo<std::u16string>(s, Encoding::UTF_32_NATIVE, Encoding::UTF_16_NATIVE) == u"ABįņ");
}
