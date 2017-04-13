//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 29.05.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/Common/Base64.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    std::string toBase64(const std::vector<uint8_t>& data)
    {
        return Yson::toBase64(data.data(), data.size());
    }

    void test_toBase64()
    {
        Y_EQUAL(Yson::toBase64(nullptr, 0), "");
        Y_EQUAL(toBase64({0}), "AA==");
        Y_EQUAL(toBase64({0, 0}), "AAA=");
        Y_EQUAL(toBase64({0, 0, 0}), "AAAA");
        Y_EQUAL(toBase64({0, 0, 0, 0}), "AAAAAA==");
        Y_EQUAL(toBase64({1}), "AQ==");
        Y_EQUAL(toBase64({7}), "Bw==");
    }

    void doTestFromBase64(const std::string& str,
                          std::initializer_list<char> expected)
    {
        //Y_EQUAL_RANGES(fromBase64(makeStringView(str)),
        //               std::vector<char>(expected));
    }

    void test_fromBase64()
    {
        Y_CALL(doTestFromBase64("", {}));
        Y_CALL(doTestFromBase64("====", {}));
        Y_CALL(doTestFromBase64("A===", {}));
        Y_CALL(doTestFromBase64("AA==", {0}));
        Y_CALL(doTestFromBase64("AAA=", {0, 0}));
        Y_CALL(doTestFromBase64("AAAA", {0, 0, 0}));
        Y_CALL(doTestFromBase64("AAAAAA==", {0, 0, 0, 0}));
        Y_CALL(doTestFromBase64("AQ==", {1}));
        Y_CALL(doTestFromBase64("Bw==", {7}));
        //Y_THROWS(fromBase64("=A="), std::exception);
    }

    Y_TEST(test_toBase64, test_fromBase64);
}
