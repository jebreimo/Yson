//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Encoding.hpp"
#include <catch2/catch.hpp>

using namespace Yconvert;

void testDetermineEncoding(const char* s, size_t len,
                           Encoding expectedEnc, ptrdiff_t expectedOffset = 0)
{
    std::string str(s, len);
    CAPTURE(str, len, expectedEnc, expectedOffset);
    auto enc = determineEncoding(s, len);
    REQUIRE(enc.first == expectedEnc);
    REQUIRE(enc.second == expectedOffset);
}

TEST_CASE("Determine encoding from first character")
{
    testDetermineEncoding("(abc)", 5, Encoding::ASCII);
    testDetermineEncoding(u8"Å", 2, Encoding::UTF_8);
    testDetermineEncoding("\0(\xAA\xAA", 4, Encoding::UTF_16_BE);
    testDetermineEncoding("\xAA\0", 2, Encoding::UTF_16_LE);
    testDetermineEncoding("\0\0\xAA\0", 4, Encoding::UTF_32_BE);
    testDetermineEncoding("QZ\0\0", 4, Encoding::UTF_32_LE);
    testDetermineEncoding("QVZ\0", 4, Encoding::UNKNOWN);
    testDetermineEncoding("\0QV", 3, Encoding::UNKNOWN);
    testDetermineEncoding("\xEFQ", 2, Encoding::UNKNOWN);
    testDetermineEncoding("\xCF", 1, Encoding::UNKNOWN);
    testDetermineEncoding("\0", 1, Encoding::UNKNOWN);
    testDetermineEncoding("", 0, Encoding::UNKNOWN);
    testDetermineEncoding(nullptr, 0, Encoding::UNKNOWN);
}

TEST_CASE("Ambiguous UTF-8")
{
    // Adding this test because I'm not sure I want this to succeed.
    testDetermineEncoding("Q\xC0", 2, Encoding::UTF_8);
}

TEST_CASE("Determine encoding from BOM")
{
    testDetermineEncoding("\xFE\xFFQWER", 6, Encoding::UTF_16_BE, 2);
    testDetermineEncoding("\xFF\xFE\0\0QWER", 8, Encoding::UTF_32_LE, 4);
    testDetermineEncoding("\xEF\xBB\xBFQWER", 7, Encoding::UTF_8, 3);
}
