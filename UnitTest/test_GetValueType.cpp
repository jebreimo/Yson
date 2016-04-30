//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/GetValueType.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace {
    using namespace Yson;

    void test(const std::string& s, ValueType_t expectedType)
    {
        auto result = getValueType(s);
        Y_EQUAL(result, expectedType);
    }

    void test_getValueType()
    {
        test("", ValueType::INVALID);
        test("0", ValueType::INTEGER);
        test("1", ValueType::INTEGER);
        test("+1", ValueType::INTEGER);
        test("-1", ValueType::INTEGER);
        test("*1", ValueType::INVALID);
        test("123", ValueType::INTEGER);
        test("0123", ValueType::INTEGER);
        test("0o123", ValueType::OCT_INTEGER);
        test("0x123", ValueType::HEX_INTEGER);
        test("+0x123", ValueType::HEX_INTEGER);
        test("-0x123", ValueType::HEX_INTEGER);
        test("*0x123", ValueType::INVALID);
        test("0b10", ValueType::BIN_INTEGER);
        test("0b00", ValueType::BIN_INTEGER);
        test("0b12", ValueType::INVALID);
        test("0b", ValueType::INVALID);
        test("0o", ValueType::INVALID);
        test("0x", ValueType::INVALID);
        test("123,", ValueType::INVALID);
        test("-", ValueType::INVALID);
        test("+", ValueType::INVALID);
        test("+", ValueType::INVALID);
        test("123.12", ValueType::FLOAT);
        test("123.", ValueType::FLOAT);
        test("-123.", ValueType::FLOAT);
        test("123.a", ValueType::INVALID);
        test("123.e", ValueType::INVALID);
        test("123.e-", ValueType::INVALID);
        test("123.e-2", ValueType::FLOAT);
        test("123.e--2", ValueType::INVALID);
        test("true", ValueType::TRUE_VALUE);
        test("false", ValueType::FALSE_VALUE);
        test("null", ValueType::NULL_VALUE);
        test("tru", ValueType::INVALID);
        test("fals", ValueType::INVALID);
        test("nul", ValueType::INVALID);
        test("truf", ValueType::INVALID);
        test("falsf", ValueType::INVALID);
        test("nulm", ValueType::INVALID);
        test("nan", ValueType::EXTENDED_FLOAT);
        test("infinity", ValueType::EXTENDED_FLOAT);
        test("-infinity", ValueType::EXTENDED_FLOAT);
        test("+infinity", ValueType::EXTENDED_FLOAT);
    }

    Y_TEST(test_getValueType);
}
