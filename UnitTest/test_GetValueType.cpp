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
        Y_CALL(test("", ValueType::INVALID));
        Y_CALL(test("0", ValueType::INTEGER));
        Y_CALL(test("1", ValueType::INTEGER));
        Y_CALL(test("+1", ValueType::INTEGER));
        Y_CALL(test("-1", ValueType::INTEGER));
        Y_CALL(test("*1", ValueType::INVALID));
        Y_CALL(test("123", ValueType::INTEGER));
        Y_CALL(test("0123", ValueType::INTEGER));
        Y_CALL(test("0o123", ValueType::OCT_INTEGER));
        Y_CALL(test("0O123", ValueType::OCT_INTEGER));
        Y_CALL(test("0x123", ValueType::HEX_INTEGER));
        Y_CALL(test("+0x123", ValueType::HEX_INTEGER));
        Y_CALL(test("-0x123", ValueType::HEX_INTEGER));
        Y_CALL(test("*0x123", ValueType::INVALID));
        Y_CALL(test("*0X123", ValueType::INVALID));
        Y_CALL(test("0b10", ValueType::BIN_INTEGER));
        Y_CALL(test("0b00", ValueType::BIN_INTEGER));
        Y_CALL(test("0B01", ValueType::BIN_INTEGER));
        Y_CALL(test("0b12", ValueType::INVALID));
        Y_CALL(test("0b", ValueType::INVALID));
        Y_CALL(test("0o", ValueType::INVALID));
        Y_CALL(test("0x", ValueType::INVALID));
        Y_CALL(test("123,", ValueType::INVALID));
        Y_CALL(test("-", ValueType::INVALID));
        Y_CALL(test("+", ValueType::INVALID));
        Y_CALL(test("+", ValueType::INVALID));
        Y_CALL(test("123.12", ValueType::FLOAT));
        Y_CALL(test("123.", ValueType::FLOAT));
        Y_CALL(test("-123.", ValueType::FLOAT));
        Y_CALL(test("123.a", ValueType::INVALID));
        Y_CALL(test("123.e", ValueType::INVALID));
        Y_CALL(test("123.e-", ValueType::INVALID));
        Y_CALL(test("123.e-2", ValueType::FLOAT));
        Y_CALL(test("123.e--2", ValueType::INVALID));
        Y_CALL(test("true", ValueType::TRUE_VALUE));
        Y_CALL(test("false", ValueType::FALSE_VALUE));
        Y_CALL(test("null", ValueType::NULL_VALUE));
        Y_CALL(test("tru", ValueType::INVALID));
        Y_CALL(test("fals", ValueType::INVALID));
        Y_CALL(test("nul", ValueType::INVALID));
        Y_CALL(test("truf", ValueType::INVALID));
        Y_CALL(test("falsf", ValueType::INVALID));
        Y_CALL(test("nulm", ValueType::INVALID));
        Y_CALL(test("NaN", ValueType::EXTENDED_FLOAT));
        Y_CALL(test("infinity", ValueType::EXTENDED_FLOAT));
        Y_CALL(test("-infinity", ValueType::EXTENDED_FLOAT));
        Y_CALL(test("+infinity", ValueType::EXTENDED_FLOAT));
    }

    Y_TEST(test_getValueType);
}
