//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Encoding.hpp"
#include "U8Adapter.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Yconvert;

void test_determine_encoding(const char* s, size_t len,
                             Encoding expected_enc,
                             ptrdiff_t expected_offset = 0)
{
    std::string str(s, len);
    CAPTURE(str, len, expected_enc, expected_offset);
    auto enc = determine_encoding(s, len);
    REQUIRE(enc.first == expected_enc);
    REQUIRE(enc.second == expected_offset);
}

TEST_CASE("Determine encoding from first character")
{
    test_determine_encoding("(abc)", 5, Encoding::UTF_8);
    test_determine_encoding(U8("Å"), 2, Encoding::UTF_8);
    test_determine_encoding("\0(\xAA\xAA", 4, Encoding::UTF_16_BE);
    test_determine_encoding("\xAA\0", 2, Encoding::UTF_16_LE);
    test_determine_encoding("\0\0\xAA\0", 4, Encoding::UTF_32_BE);
    test_determine_encoding("QZ\0\0", 4, Encoding::UTF_32_LE);
    test_determine_encoding("QVZ\0", 4, Encoding::UNKNOWN);
    test_determine_encoding("\0QV", 3, Encoding::UNKNOWN);
    test_determine_encoding("\xEFQ", 2, Encoding::UNKNOWN);
    test_determine_encoding("\xCF", 1, Encoding::UNKNOWN);
    test_determine_encoding("\0", 1, Encoding::UNKNOWN);
    test_determine_encoding("", 0, Encoding::UNKNOWN);
    test_determine_encoding(nullptr, 0, Encoding::UNKNOWN);
}

TEST_CASE("Ambiguous UTF-8")
{
    // Adding this test because I'm not sure if I want this to succeed.
    test_determine_encoding("Q\xC0", 2, Encoding::UTF_8);
}

TEST_CASE("Determine encoding from BOM")
{
    test_determine_encoding("\xFE\xFFQWER", 6, Encoding::UTF_16_BE, 2);
    test_determine_encoding("\xFF\xFE\0\0QWER", 8, Encoding::UTF_32_LE, 4);
    test_determine_encoding("\xEF\xBB\xBFQWER", 7, Encoding::UTF_8, 3);
}
