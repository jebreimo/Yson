//****************************************************************************
// Copyright © 2018 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2018-12-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/Common/IsJavaScriptIdentifier.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_isJavaScriptIdentifier()
    {
        Y_ASSERT(isJavaScriptIdentifier("fooBar123"));
        Y_ASSERT(!isJavaScriptIdentifier("+fooBar123"));
        Y_ASSERT(!isJavaScriptIdentifier("123FooBar"));
        Y_ASSERT(isJavaScriptIdentifier("f\\uAB12"));
        Y_ASSERT(isJavaScriptIdentifier("\\uAb12k"));
        Y_ASSERT(!isJavaScriptIdentifier("\\uAb1k"));
    }

    Y_TEST(test_isJavaScriptIdentifier);
}
