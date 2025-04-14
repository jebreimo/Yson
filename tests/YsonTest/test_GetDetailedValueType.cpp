//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/Common/GetDetailedValueType.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test(const std::string& s, DetailedValueType expectedType)
    {
        auto result = getDetailedValueType(s);
        Y_EQUAL(result, expectedType);
    }

    void test_getDetailedValueType()
    {
        Y_CALL(test("", DetailedValueType::INVALID));
        Y_CALL(test("0", DetailedValueType::UINT_7));
        Y_CALL(test("-0", DetailedValueType::UINT_7));
        Y_CALL(test("1", DetailedValueType::UINT_7));
        Y_CALL(test("*1", DetailedValueType::INVALID));

        Y_CALL(test("-1_1", DetailedValueType::SINT_8));
        Y_CALL(test("-_1_1", DetailedValueType::INVALID));
        Y_CALL(test("-1__1", DetailedValueType::INVALID));
        Y_CALL(test("-1_1_", DetailedValueType::INVALID));

        Y_CALL(test("+11.11e-11", DetailedValueType::FLOAT_64));
        Y_CALL(test("+1_1.1_1e-1_1", DetailedValueType::FLOAT_64));
        Y_CALL(test("+_1_1.1_1e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1__1.1_1e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1_.1_1e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1._1_1e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1.1__1e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1.1_1_e1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1.1_1e_1_1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1.1_1e1__1", DetailedValueType::INVALID));
        Y_CALL(test("+1_1.1_1e1_1_", DetailedValueType::INVALID));

        Y_CALL(test("-2147483649", DetailedValueType::SINT_64));
        Y_CALL(test("-2_147_483_648", DetailedValueType::SINT_32));
        Y_CALL(test("-327690", DetailedValueType::SINT_32));
        Y_CALL(test("-32769", DetailedValueType::SINT_32));
        Y_CALL(test("-32768", DetailedValueType::SINT_16));
        Y_CALL(test("-1290", DetailedValueType::SINT_16));
        Y_CALL(test("-129", DetailedValueType::SINT_16));
        Y_CALL(test("-128", DetailedValueType::SINT_8));
        Y_CALL(test("-99", DetailedValueType::SINT_8));
        Y_CALL(test("-1", DetailedValueType::SINT_8));
        Y_CALL(test("+1", DetailedValueType::UINT_7));
        Y_CALL(test("99", DetailedValueType::UINT_7));
        Y_CALL(test("127", DetailedValueType::UINT_7));
        Y_CALL(test("128", DetailedValueType::UINT_8));
        Y_CALL(test("255", DetailedValueType::UINT_8));
        Y_CALL(test("256", DetailedValueType::UINT_15));
        Y_CALL(test("1256", DetailedValueType::UINT_15));
        Y_CALL(test("32767", DetailedValueType::UINT_15));
        Y_CALL(test("32768", DetailedValueType::UINT_16));
        Y_CALL(test("65535", DetailedValueType::UINT_16));
        Y_CALL(test("65536", DetailedValueType::UINT_31));
        Y_CALL(test("327680", DetailedValueType::UINT_31));
        Y_CALL(test("327680000", DetailedValueType::UINT_31));
        Y_CALL(test("2147483647", DetailedValueType::UINT_31));
        Y_CALL(test("2147483648", DetailedValueType::UINT_32));
        Y_CALL(test("4294967295", DetailedValueType::UINT_32));
        Y_CALL(test("4_294_967_296", DetailedValueType::UINT_63));

        Y_CALL(test("0000123", DetailedValueType::UINT_7));
        Y_CALL(test("0o123", DetailedValueType::UINT_7));
        Y_CALL(test("0O123", DetailedValueType::UINT_7));
        Y_CALL(test("0x123ABC", DetailedValueType::UINT_31));
        Y_CALL(test("0X123ABC", DetailedValueType::UINT_31));
        Y_CALL(test("+0x123", DetailedValueType::UINT_15));
        Y_CALL(test("-0x123", DetailedValueType::SINT_16));
        Y_CALL(test("*0x123", DetailedValueType::INVALID));
        Y_CALL(test("*0X123", DetailedValueType::INVALID));
        Y_CALL(test("-0b1000_0000", DetailedValueType::SINT_8));
        Y_CALL(test("-0b1000_0001", DetailedValueType::SINT_16));
        Y_CALL(test("0b10", DetailedValueType::UINT_7));
        Y_CALL(test("0b00", DetailedValueType::UINT_7));
        Y_CALL(test("0B01", DetailedValueType::UINT_7));
        Y_CALL(test("0b12", DetailedValueType::INVALID));
        Y_CALL(test("0b", DetailedValueType::INVALID));
        Y_CALL(test("0o", DetailedValueType::INVALID));
        Y_CALL(test("0x", DetailedValueType::INVALID));
        Y_CALL(test("123,", DetailedValueType::INVALID));
        Y_CALL(test("-", DetailedValueType::INVALID));
        Y_CALL(test("+", DetailedValueType::INVALID));
        Y_CALL(test("+", DetailedValueType::INVALID));
        Y_CALL(test("123.12", DetailedValueType::FLOAT_64));
        Y_CALL(test("123.", DetailedValueType::FLOAT_64));
        Y_CALL(test("-123.", DetailedValueType::FLOAT_64));
        Y_CALL(test("123.a", DetailedValueType::INVALID));
        Y_CALL(test("123.e", DetailedValueType::INVALID));
        Y_CALL(test("123.e-", DetailedValueType::INVALID));
        Y_CALL(test("123.e-2", DetailedValueType::FLOAT_64));
        Y_CALL(test("123.e--2", DetailedValueType::INVALID));
        Y_CALL(test("true", DetailedValueType::BOOLEAN));
        Y_CALL(test("false", DetailedValueType::BOOLEAN));
        Y_CALL(test("null", DetailedValueType::NULL_VALUE));
        Y_CALL(test("tru", DetailedValueType::INVALID));
        Y_CALL(test("fals", DetailedValueType::INVALID));
        Y_CALL(test("nul", DetailedValueType::INVALID));
        Y_CALL(test("truf", DetailedValueType::INVALID));
        Y_CALL(test("falsf", DetailedValueType::INVALID));
        Y_CALL(test("nulm", DetailedValueType::INVALID));
        Y_CALL(test("NaN", DetailedValueType::FLOAT_64));
        Y_CALL(test("Infinity", DetailedValueType::FLOAT_64));
        Y_CALL(test("-Infinity", DetailedValueType::FLOAT_64));
        Y_CALL(test("+Infinity", DetailedValueType::FLOAT_64));

        Y_CALL(test("-0x007F", DetailedValueType::SINT_8));
        Y_CALL(test("-0x0_0_7_F", DetailedValueType::SINT_8));
        Y_CALL(test("-0x_0_0_7_F", DetailedValueType::INVALID));
        Y_CALL(test("-0x0__0_7_F", DetailedValueType::INVALID));
        Y_CALL(test("-0x0_0__7_F", DetailedValueType::INVALID));
        Y_CALL(test("-0x0_0_7__F", DetailedValueType::INVALID));
        Y_CALL(test("-0x0_0_7_F_", DetailedValueType::INVALID));
    }

    Y_TEST(test_getDetailedValueType);
}
