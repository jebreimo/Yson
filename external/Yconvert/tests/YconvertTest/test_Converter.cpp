//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Converter.hpp"
#include "U8Adapter.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Yconvert;

TEST_CASE("Converter with UTF-8 -> UTF-8")
{
    Converter converter(Encoding::UTF_8, Encoding::UTF_8);
    std::string s(U8("AäöØ∂ƒ‹‘"));
    REQUIRE(converter.get_encoded_size(s.data(), s.size()) == 21);
    std::string t(21, '\0');
    auto [m, n] = converter.convert(s.data(), s.size(), t.data(), t.size());
    REQUIRE(m == 21);
    REQUIRE(n == 21);
    REQUIRE(t == s);
}

TEST_CASE("Converter with UTF-16 -> UTF-8")
{
    Converter converter(Encoding::UTF_16_NATIVE, Encoding::UTF_8);
    std::u16string s(u"AäöØ∂ƒ‹‘");
    REQUIRE(converter.get_encoded_size(s.data(), s.size() * 2) == 21);
    std::string t(21, '\0');
    auto[m, n] = converter.convert(s.data(), s.size() * 2, t.data(), t.size());
    REQUIRE(m == s.size() * 2);
    REQUIRE(n == 21);
    REQUIRE(t == U8("AäöØ∂ƒ‹‘"));
}

TEST_CASE("Converter with UTF-16BE -> UTF-16LE")
{
    auto BE = [](uint16_t c){return get_big_endian(c);};
    auto LE = [](uint16_t c) {return get_little_endian(c);};

    Converter converter(Encoding::UTF_16_BE, Encoding::UTF_16_LE);
    std::vector<uint16_t> s{BE('A'), BE(0xD900), BE(0xDD00)};
    REQUIRE(converter.get_encoded_size(s.data(), s.size() * 2) == 6);
    std::vector<uint16_t> t(s.size());
    auto[m, n] = converter.convert(s.data(), s.size() * 2, t.data(), t.size() * 2);
    REQUIRE(m == s.size() * 2);
    REQUIRE(n == t.size() * 2);
    REQUIRE(t[1] == LE(0xD900));
}

TEST_CASE("Converter with UTF-16 -> iso8859-1")
{
    Converter converter(Encoding::UTF_16_NATIVE, Encoding::ISO_8859_1);
    std::u16string s(u"AäöØõ");
    REQUIRE(converter.get_encoded_size(s.data(), s.size() * 2) == 6);
    std::string t;
    auto n = converter.convert(s.data(), s.size() * 2, t);
    REQUIRE(n == s.size() * 2);
    REQUIRE(t == "A\xE4\xF6?\xD8\xF5");
}

TEST_CASE("Converter with iso8859-1 -> UTF-16")
{
    Converter converter(Encoding::ISO_8859_1, Encoding::UTF_16_NATIVE);
    std::string s("A\xE4\xF6?\xD8\xF5");
    REQUIRE(converter.get_encoded_size(s.data(), s.size()) == 12);
    std::u16string t(6, u'\0');
    auto [m, n] = converter.convert(s.data(), s.size(), t.data(), t.size() * 2);
    REQUIRE(m == s.size());
    REQUIRE(n == t.size() * 2);
    REQUIRE(t == u"Aäö?Øõ");
}
