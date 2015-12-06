//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 06.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/JsonReader.hpp"

#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

using namespace Yson;

template <typename T>
void testReadOverflow(const std::string& doc)
{
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
    T n;
    Y_THROWS(reader.read(n), JsonReaderException);
}

template<typename T>
void testRead(const std::string& doc, T expectedValue)
{
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
    T n;
    Y_NO_THROW(reader.read(n), JsonReaderException);
    Y_EQUAL(n, expectedValue);
}

void test_IntegerOverflow()
{
    Y_CALL(testRead<int64_t>("999888777666555", 999888777666555));
    Y_CALL(testReadOverflow<int32_t>("999888777666555"));
}

Y_TEST(test_IntegerOverflow);
