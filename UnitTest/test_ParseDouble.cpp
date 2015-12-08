//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/ParseDouble.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace {
    using namespace Yson;

    void success(const std::string& s, double expectedValue)
    {
        auto result = parseDouble(s.data(), s.data() + s.size());
        Y_EQUAL(result.second, true);
        Y_EQUIVALENT(result.first, expectedValue, 1e-10);
    }

    void failure(const std::string& s)
    {
        auto result = parseDouble(s.data(), s.data() + s.size());
        Y_EQUAL(result.second, false);
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
        Y_CALL(success("1234.5678e222", 1234.5678e222));
        Y_CALL(success("1234.5678e-222", 1234.5678e-222));
        Y_CALL(success("1234.5678e+222", 1234.5678e222));
        Y_CALL(success("-1234.5678e222", -1234.5678e222));
        Y_CALL(success("-1234.5678e-222", -1234.5678e-222));
        Y_CALL(success("-1234.5678e+222", -1234.5678e222));
        Y_CALL(success("1e308", 1e308));
        Y_CALL(failure("1e309"));
        Y_CALL(success("1e-308", 1e-308));
        Y_CALL(failure("1e-309"));
    }

    Y_TEST(test_parseDouble);
}
