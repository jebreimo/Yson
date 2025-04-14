//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/Common/GetValueType.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test(const std::string& s, ValueType expectedType)
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
        Y_CALL(test("2017_02_17", ValueType::INTEGER));
        Y_CALL(test("0123", ValueType::INTEGER));
        Y_CALL(test("0o123", ValueType::INTEGER));
        Y_CALL(test("0O123", ValueType::INTEGER));
        Y_CALL(test("123_456_789", ValueType::INTEGER));
        Y_CALL(test("0x12_3ABC", ValueType::INTEGER));
        Y_CALL(test("0X12_3ABC", ValueType::INTEGER));
        Y_CALL(test("0x123", ValueType::INTEGER));
        Y_CALL(test("+0x123", ValueType::INTEGER));
        Y_CALL(test("-0x123", ValueType::INTEGER));
        Y_CALL(test("*0x123", ValueType::INVALID));
        Y_CALL(test("*0X123", ValueType::INVALID));
        Y_CALL(test("_", ValueType::INVALID));
        Y_CALL(test("_1", ValueType::INVALID));
        Y_CALL(test("1_", ValueType::INVALID));
        Y_CALL(test("1_1", ValueType::INTEGER));
        Y_CALL(test("1__1", ValueType::INVALID));
        Y_CALL(test("0b10", ValueType::INTEGER));
        Y_CALL(test("0b10_10", ValueType::INTEGER));
        Y_CALL(test("0b10_10_", ValueType::INVALID));
        Y_CALL(test("0b10__10", ValueType::INVALID));
        Y_CALL(test("0b00", ValueType::INTEGER));
        Y_CALL(test("0B01", ValueType::INTEGER));
        Y_CALL(test("0b12", ValueType::INVALID));
        Y_CALL(test("0b", ValueType::INVALID));
        Y_CALL(test("0o", ValueType::INVALID));
        Y_CALL(test("0x", ValueType::INVALID));
        Y_CALL(test("123,", ValueType::INVALID));
        Y_CALL(test("-", ValueType::INVALID));
        Y_CALL(test("+", ValueType::INVALID));
        Y_CALL(test("+", ValueType::INVALID));
        Y_CALL(test("123.12", ValueType::FLOAT));
        Y_CALL(test("123_456.123456", ValueType::FLOAT));
        Y_CALL(test("123_456_.123456", ValueType::INVALID));
        Y_CALL(test("123_456.123_456", ValueType::FLOAT));
        Y_CALL(test("123_456._123_456", ValueType::INVALID));
        Y_CALL(test("123_456.123_e456", ValueType::INVALID));
        Y_CALL(test("123_456.123e_456", ValueType::INVALID));
        Y_CALL(test("123.", ValueType::FLOAT));
        Y_CALL(test("-123.", ValueType::FLOAT));
        Y_CALL(test("123.a", ValueType::INVALID));
        Y_CALL(test("123.e", ValueType::INVALID));
        Y_CALL(test("123.e-", ValueType::INVALID));
        Y_CALL(test("123.e-2", ValueType::FLOAT));
        Y_CALL(test("123.e--2", ValueType::INVALID));
        Y_CALL(test("true", ValueType::BOOLEAN));
        Y_CALL(test("false", ValueType::BOOLEAN));
        Y_CALL(test("null", ValueType::NULL_VALUE));
        Y_CALL(test("tru", ValueType::INVALID));
        Y_CALL(test("fals", ValueType::INVALID));
        Y_CALL(test("nul", ValueType::INVALID));
        Y_CALL(test("truf", ValueType::INVALID));
        Y_CALL(test("falsf", ValueType::INVALID));
        Y_CALL(test("nulm", ValueType::INVALID));
        Y_CALL(test("NaN", ValueType::FLOAT));
        Y_CALL(test("Infinity", ValueType::FLOAT));
        Y_CALL(test("-Infinity", ValueType::FLOAT));
        Y_CALL(test("+Infinity", ValueType::FLOAT));

        Y_CALL(test("+11.11e-11", ValueType::FLOAT));
        Y_CALL(test("+1_1.1_1e-1_1", ValueType::FLOAT));
        Y_CALL(test("+_1_1.1_1e1_1", ValueType::INVALID));
        Y_CALL(test("+1__1.1_1e1_1", ValueType::INVALID));
        Y_CALL(test("+1_1_.1_1e1_1", ValueType::INVALID));
        Y_CALL(test("+1_1._1_1e1_1", ValueType::INVALID));
        Y_CALL(test("+1_1.1__1e1_1", ValueType::INVALID));
        Y_CALL(test("+1_1.1_1_e1_1", ValueType::INVALID));
        Y_CALL(test("+1_1.1_1e_1_1", ValueType::INVALID));
        Y_CALL(test("+1_1.1_1e1__1", ValueType::INVALID));
        Y_CALL(test("+1_1.1_1e1_1_", ValueType::INVALID));
    }

    Y_TEST(test_getValueType);
}
