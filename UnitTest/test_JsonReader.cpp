//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-06.
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

template <typename T>
void read(const std::string& doc, T& value)
{
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.read(value), JsonReaderException);
}

template<typename T>
void testRead(const std::string& doc, T expectedValue)
{
    T n;
    Y_CALL(read(doc, n));
    Y_EQUAL(n, expectedValue);
}

void testRead(const std::string& doc, double expectedValue, double e = 1e-10)
{
    double n;
    Y_CALL(read(doc, n));
    Y_EQUIVALENT(n, expectedValue, e);
}

void testRead(const std::string& doc, float expectedValue, float e = 1e-3)
{
    double n;
    Y_CALL(read(doc, n));
    Y_EQUIVALENT(n, expectedValue, e);
}

void test_IntegerOverflow()
{
    Y_CALL(testReadOverflow<int32_t>("2147483648"));
    Y_CALL(testReadOverflow<int32_t>("-2147483649"));
}

void test_Integer()
{
    Y_CALL(testRead<int32_t>("2147483647", 2147483647));
    Y_CALL(testRead<int32_t>("-2147483648", -2147483648));
    Y_CALL(testRead<int64_t>("9223372036854775807", 9223372036854775807LL));
    Y_CALL(testRead<uint64_t>("18446744073709551615", 18446744073709551615ULL));
}

void test_Double()
{
    Y_CALL(testRead("21474.83647", 21474.83647));
}

void test_Float()
{
    Y_CALL(testRead("21474.836", 21474.836f));
}

void test_EnterNull()
{
    std::string doc = "null";
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
//    Y_NO_THROW(reader.enter(), JsonReaderException);
//    Y_ASSERT(!reader.nextKey());
//    Y_ASSERT(!reader.nextValue());
//    Y_NO_THROW(reader.leave(), JsonReaderException);
//    Y_ASSERT(!reader.nextValue());
}
Y_TEST(test_IntegerOverflow,
       test_Integer,
       test_Double,
       test_EnterNull);
