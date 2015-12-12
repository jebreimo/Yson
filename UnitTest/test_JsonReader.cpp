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
void testReadFails(const std::string& doc)
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

void test_IntegerError()
{
    Y_CALL(testReadFails<int8_t>("-129"));
    Y_CALL(testReadFails<int8_t>("128"));
    Y_CALL(testReadFails<uint8_t>("-1"));
    Y_CALL(testReadFails<uint8_t>("256"));
    Y_CALL(testReadFails<int16_t>("-32769"));
    Y_CALL(testReadFails<int16_t>("32768"));
    Y_CALL(testReadFails<uint16_t>("-1"));
    Y_CALL(testReadFails<uint16_t>("65536"));
    Y_CALL(testReadFails<int32_t>("2147483648"));
    Y_CALL(testReadFails<int32_t>("-2147483649"));
    Y_CALL(testReadFails<uint32_t>("4294967296"));
    Y_CALL(testReadFails<uint32_t>("-1"));
    Y_CALL(testReadFails<uint64_t>("-1"));
}

void test_Integer()
{
    Y_CALL(testRead<int8_t>("-128", -128));
    Y_CALL(testRead<int8_t>("127", 127));
    Y_CALL(testRead<uint8_t>("255", 255));
    Y_CALL(testRead<int16_t>("-32768", -32768));
    Y_CALL(testRead<int16_t>("32767", 32767));
    Y_CALL(testRead<uint16_t>("65535", 65535));
    Y_CALL(testRead<int32_t>("2147483647", 2147483647));
    Y_CALL(testRead<int32_t>("-2147483648", -2147483648));
    Y_CALL(testRead<uint32_t>("4294967295", 4294967295u));
    Y_CALL(testRead<int64_t>("9223372036854775807", 9223372036854775807LL));
    Y_CALL(testRead<uint64_t>("18446744073709551615",
                              18446744073709551615ULL));
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
    reader.setEnterNullEnabled(true);
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_ASSERT(!reader.nextKey());
    Y_ASSERT(!reader.nextValue());
    Y_NO_THROW(reader.leave(), JsonReaderException);
    Y_ASSERT(!reader.nextValue());
}

void test_NextDocument()
{
    std::string doc = "1 {\"2\": []} 3";
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<int32_t>(reader), 1);
    Y_ASSERT(!reader.nextValue());

    Y_ASSERT(reader.nextDocument());
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_ASSERT(reader.nextKey());
    Y_EQUAL(read<std::string>(reader), "2");
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);

    Y_ASSERT(reader.nextDocument());
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<double>(reader), 3);
    Y_ASSERT(!reader.nextValue());

    Y_ASSERT(reader.nextDocument());
    Y_ASSERT(!reader.nextValue());
    Y_ASSERT(!reader.nextDocument());
}

void test_RecoverFromError()
{
    std::string doc = "[{\"1\": [{23: 4, : 4}],,}, {\"2\": 3}]";
    std::istringstream ss(doc);
    JsonReader reader(ss);
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_NO_THROW(reader.nextValue(), JsonReaderException);
    Y_THROWS(reader.nextValue(), JsonReaderException);
    Y_THROWS(reader.nextValue(), JsonReaderException);
    Y_THROWS(reader.nextValue(), JsonReaderException);
    Y_THROWS(reader.nextValue(), JsonReaderException);
    Y_NO_THROW(reader.nextValue(), JsonReaderException);
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_NO_THROW(reader.nextKey(), JsonReaderException);
    Y_EQUAL(read<std::string>(reader), "2");
    Y_NO_THROW(reader.leave(), JsonReaderException);
    Y_ASSERT(!reader.nextValue());
};

void test_ValuesAsStrings()
{
    std::string doc = "[null, 12.34, fooz, \"baz\"]";
    std::istringstream ss(doc);
    JsonReader reader(ss);
    reader.setValuesAsStringsEnabled(true);
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<std::string>(reader), "null");
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<std::string>(reader), "12.34");
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<std::string>(reader), "fooz");
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<std::string>(reader), "baz");
    Y_ASSERT(!reader.nextValue());
    Y_NO_THROW(reader.leave(), JsonReaderException);
    Y_ASSERT(!reader.nextValue());

    std::istringstream ss2(doc);
    JsonReader reader2(ss2);
    Y_ASSERT(reader2.nextValue());
    Y_NO_THROW(reader2.enter(), JsonReaderException);
    Y_ASSERT(reader2.nextValue());
    Y_THROWS(read<std::string>(reader2), JsonReaderException);
}

void test_BlockString()
{
    std::string doc = "{\"multi-line text\": \"\"\"This is a\n"
            "multi-line text!\"\"\"}";
    std::istringstream ss(doc);
    JsonReader reader(ss);
    reader.setBlockStringsEnabled(true);
    Y_ASSERT(reader.nextValue());
    Y_NO_THROW(reader.enter(), JsonReaderException);
    Y_ASSERT(reader.nextKey());
    Y_EQUAL(read<std::string>(reader), "multi-line text");
    Y_ASSERT(reader.nextValue());
    Y_EQUAL(read<std::string>(reader), "This is a\nmulti-line text!");
    Y_EQUAL(reader.lineNumber(), 2);
    Y_ASSERT(!reader.nextKey());
    Y_NO_THROW(reader.leave(), JsonReaderException);
    Y_ASSERT(!reader.nextValue());
}

Y_TEST(test_IntegerError,
       test_Integer,
       test_Double,
       test_Float,
       test_EnterNull,
       test_NextDocument,
       test_RecoverFromError,
       test_ValuesAsStrings,
       test_BlockString);
