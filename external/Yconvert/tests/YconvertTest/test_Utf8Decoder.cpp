//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf8Decoder.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Test Utf8Decoder")
{
    std::string s(u8"AÆΩ" "\xE0\x80 F");
    Yconvert::Utf8Decoder decoder;
    std::vector<char32_t> u(6);
    SECTION("Replace")
    {
        REQUIRE(decoder.decode(s.data(), s.size(), u.data(), u.size())
                == std::pair<size_t, size_t>(9, 6));
        REQUIRE(u == std::vector<char32_t>{
            'A', U'Æ', U'Ω', Yconvert::REPLACEMENT_CHARACTER, ' ', 'F'});
    }
    SECTION("Skip")
    {
        decoder.setErrorHandlingPolicy(Yconvert::ErrorPolicy::SKIP);
        REQUIRE(decoder.decode(s.data(), s.size(), u.data(), u.size())
                == std::pair<size_t, size_t>(9, 5));
        REQUIRE(u == std::vector<char32_t>{'A', U'Æ', U'Ω', ' ', 'F', 0});
    }
    SECTION("Throw")
    {
        decoder.setErrorHandlingPolicy(Yconvert::ErrorPolicy::THROW);
        REQUIRE_THROWS(decoder.decode(s.data(), s.size(), u.data(), u.size()));
    }
}
