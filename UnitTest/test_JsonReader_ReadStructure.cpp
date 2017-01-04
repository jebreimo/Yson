//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/JsonReader.hpp"

#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_fromStream()
    {
        auto doc = R"({"int": 1234, "array": [1, 2, {"float": 1.234}]})";
        std::istringstream iss(doc);
        JsonReader reader(iss);
        auto object = reader.read();
        Y_EQUAL(getValue<int>((*object)["int"]), 1234);
        Y_EQUAL(getValue<unsigned>((*object)["array"][0]), 1);
        Y_EQUAL(getValue<unsigned>((*object)["array"][1]), 2);
        Y_EQUAL(getValue<double>((*object)["array"][2]["float"]), 1.234);
    }

    void test_fromBuffer()
    {
        auto doc = R"({"int": 1234, "array": [1, 2, {"float": 1.234}]})";
        JsonReader reader(doc, strlen(doc));
        auto object = reader.read();
        Y_EQUAL(getValue<int>((*object)["int"]), 1234);
        Y_EQUAL(getValue<unsigned>((*object)["array"][0]), 1);
        Y_EQUAL(getValue<unsigned>((*object)["array"][1]), 2);
        Y_EQUAL(getValue<double>((*object)["array"][2]["float"]), 1.234);
    }

    Y_TEST(test_fromStream, test_fromBuffer);
}
