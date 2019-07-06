//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/Common/ParseFloatingPoint.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void success(const std::string& s, double expectedValue, double e = 1e-10)
    {
        double result;
        Y_ASSERT(parse(makeStringView(s), result));
        Y_EQUIVALENT(result, expectedValue, e);
    }

    void failure(const std::string& s)
    {
        double result;
        Y_EQUAL(parse(makeStringView(s), result), false);
    }

    void isNotANumber(const std::string& s)
    {
        double result;
        Y_ASSERT(parse(makeStringView(s), result));
        Y_ASSERT(isnan(result));
    }

    void isInfinite(const std::string& s, bool negative)
    {
        double result;
        Y_ASSERT(parse(makeStringView(s), result));
        Y_ASSERT(isinf(result));
        Y_EQUAL(result < 0, negative);
    }

    void test_parseDouble()
    {
        Y_CALL(failure(""));
        Y_CALL(failure(" "));
        Y_CALL(success("0", 0));
        Y_CALL(success("1.", 1));
        Y_CALL(failure("0 "));
        Y_CALL(failure(" 0"));
        Y_CALL(failure("."));
        Y_CALL(failure(".0"));
        Y_CALL(failure("e"));
        Y_CALL(failure("+"));
        Y_CALL(failure("-"));
        Y_CALL(failure("-.2"));
        Y_CALL(failure("-e2"));
        Y_CALL(success("1.2", 1.2));
        Y_CALL(success("123456.123456", 123456.123456));
        Y_CALL(success("-123456.123456", -123456.123456));
        Y_CALL(success("+123456.123456", 123456.123456));
        Y_CALL(success("0e0", 0));
        Y_CALL(success("1E5", 1e5));
        Y_CALL(success("1.E5", 1e5));
        Y_CALL(success("-1.E5", -1e5));
        Y_CALL(failure("1.2E"));
        Y_CALL(failure("1.2E-"));
        Y_CALL(failure("1.2E "));
        Y_CALL(failure("1.2E2."));
        Y_CALL(failure("1.2E2 "));
        Y_CALL(success("1234.5678e222", 1234.5678e222, 1e215));
        Y_CALL(success("1234.5678e-222", 1234.5678e-222));
        Y_CALL(success("1234.5678e+222", 1234.5678e222, 1e215));
        Y_CALL(success("-1234.5678e222", -1234.5678e222, 1e215));
        Y_CALL(success("-1234.5678e-222", -1234.5678e-222));
        Y_CALL(success("-1234.5678e+222", -1234.5678e222, 1e215));
        Y_CALL(success("-1234.5678e+222", -1234.5678e222, 1e215));
        Y_CALL(success("2.2250738585072014e-308", 2.2250738585072014e-308));
        Y_CALL(success("2.2250739e-308", 2.2250739e-308));
        Y_CALL(success("1.7976931348623157e+308", 1.7976931348623157e+308));
        Y_CALL(success("1e308", 1e308, 1e298));
        Y_CALL(failure("1e309"));
        Y_CALL(success("1e-307", 1e-307));
        Y_CALL(success("1e-308", 1e-308));
        Y_CALL(isNotANumber("NaN"));
        Y_CALL(isInfinite("null", false));
        Y_CALL(isInfinite("Infinity", false));
        Y_CALL(isInfinite("+Infinity", false));
        Y_CALL(isInfinite("-Infinity", true));
    }

    Y_TEST(test_parseDouble);
}
