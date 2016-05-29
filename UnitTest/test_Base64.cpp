//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 29.05.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/Base64.hpp"

#include "../Externals/Ytest/Ytest.hpp"

std::string toBase64(std::initializer_list<uint8_t> list)
{
    std::vector<uint8_t> data(list);
    return Yson::toBase64(data.data(), data.size());
}

void test_toBase64()
{
    Y_EQUAL(Yson::toBase64(nullptr, 0), "");
    Y_EQUAL(toBase64({0}), "AA==");
    Y_EQUAL(toBase64({0, 0}), "AAA=");
    Y_EQUAL(toBase64({0, 0, 0}), "AAAA");
}

Y_TEST(test_toBase64);
