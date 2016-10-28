//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/Reader.hpp"

#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test()
    {
        auto doc = R"({"int": 1234, "array": [1, 2, {"float": 1.234}]})";
        std::istringstream iss(doc);
        Reader reader(iss);
        auto object = reader.readStructure();
        Y_EQUAL(getValue<int>((*object)["int"]), 1234);
        Y_EQUAL(getValue<unsigned>((*object)["array"][0]), 1);
        Y_EQUAL(getValue<unsigned>((*object)["array"][1]), 2);
        Y_EQUAL(getValue<double>((*object)["array"][2]["float"]), 1.234);
    }

    Y_TEST(test);
}
