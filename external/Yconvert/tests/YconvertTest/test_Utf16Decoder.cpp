//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf16Decoder.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("Test UTF-16-BE decoder")
{
    std::string s("\0A\0B\0C\0D\0E\0F\0G", 14);
    Yconvert::Utf16Decoder<Yconvert::IS_LITTLE_ENDIAN> decoder;
    std::vector<char32_t> u(7);
    auto [m, n] = decoder.decode(s.data(), s.size(), u.data(), u.size());
    REQUIRE(m == 14);
    REQUIRE_THAT(u, Catch::Matchers::Equals(std::vector<char32_t>{'A', 'B', 'C', 'D', 'E', 'F', 'G'}));
}
