//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf8Decoder.hpp"
#include "U8Adapter.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test Utf8Decoder")
{
    std::string s(U8("AÆΩ" "\xE0\x80 F"));
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
        decoder.set_error_policy(Yconvert::ErrorPolicy::SKIP);
        REQUIRE(decoder.decode(s.data(), s.size(), u.data(), u.size())
                == std::pair<size_t, size_t>(9, 5));
        REQUIRE(u == std::vector<char32_t>{'A', U'Æ', U'Ω', ' ', 'F', 0});
    }
    SECTION("Throw")
    {
        decoder.set_error_policy(Yconvert::ErrorPolicy::THROW);
        REQUIRE_THROWS(decoder.decode(s.data(), s.size(), u.data(), u.size()));
    }
}
