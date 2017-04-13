//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/UBJsonReader.hpp"
#include "../Externals/Ytest/Ytest.hpp"
#include "../Yson/YsonException.hpp"

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        std::string doc("{i\x03KeySi\x0CHello world!"
                                "i\x05" "Array[I\x10\x20U\xF0]}");
        UBJsonReader reader(doc.data(), doc.size());
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(), ValueType::OBJECT);
        Y_EQUAL(reader.detailedValueType(), DetailedValueType::OBJECT);
        reader.enter();
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(reader.valueType(), ValueType::STRING);
        Y_EQUAL(reader.detailedValueType(), DetailedValueType::STRING);
        Y_EQUAL(read<std::string>(reader), "Key");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(true), ValueType::STRING);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::STRING);
        Y_EQUAL(read<std::string>(reader), "Hello world!");
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(reader.valueType(true), ValueType::STRING);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::STRING);
        Y_EQUAL(read<std::string>(reader), "Array");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(true), ValueType::ARRAY);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::ARRAY);
        reader.enter();
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(true), ValueType::INTEGER);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::UINT_15);
        Y_EQUAL(read<int16_t>(reader), 0x1020);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(true), ValueType::INTEGER);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::UINT_8);
        Y_EQUAL(read<int64_t>(reader), 240);
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        reader.leave();
        Y_ASSERT(!reader.nextKey());
        Y_ASSERT(!reader.nextKey());
        reader.leave();
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextValue());
    }

    void assert_throws(const std::string& doc)
    {
        UBJsonReader reader(doc.data(), doc.size());
        Y_THROWS(reader.nextValue(), YsonException);
    }

    void test_NextDocumentValue()
    {
        assert_throws("]");
        assert_throws("}");
        assert_throws("A");
        assert_throws("0");
    }

    template <typename T, size_t N>
    void readSuccedes(const char(&doc)[N], T expectedValue)
    {
        UBJsonReader reader(doc, sizeof(doc) - 1);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<T>(reader), expectedValue);
    }

    template <typename T>
    void readSuccedes(const std::string& doc, T expectedValue)
    {
        UBJsonReader reader(doc.data(), doc.size());
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<T>(reader), expectedValue);
    }

    template <typename T>
    void readFails(const std::string& doc)
    {
        UBJsonReader reader(doc.data(), doc.size());
        Y_ASSERT(reader.nextValue());
        T value;
        Y_ASSERT(!reader.read(value));
    }

    void test_Read()
    {
        Y_CALL(readSuccedes<int8_t>("i\x80", -128));
        Y_CALL(readSuccedes<int16_t>("i\x80", -128));
        Y_CALL(readFails<uint8_t>("i\x80"));
        Y_CALL(readFails<uint16_t>("i\x80"));

        Y_CALL(readSuccedes<uint8_t>("U\x7F", 127));
        Y_CALL(readSuccedes<int8_t>("U\x7F", 127));
        Y_CALL(readSuccedes<uint8_t>("U\x80", 128));
        Y_CALL(readSuccedes<int16_t>("U\x80", 128));
        Y_CALL(readFails<int8_t>("U\x80"));

        Y_CALL(readSuccedes<int8_t>("i\xFF", -1));
        Y_CALL(readFails<uint8_t>("i\xFF"));
        Y_CALL(readFails<uint64_t>("i\xFF"));

        Y_CALL(readSuccedes<float>("d\x41\x8c\x00\x00", 17.5));

        Y_CALL(readSuccedes<int16_t>("Si\x05""32767", 32767));
    }

    void test_OptimizedArray()
    {
        char doc[] = "[$d#i\x03" "\x41\x8c\x00\x00" "\x41\x8a\x00\x00"
                "\x41\x89\x00\x00";
        UBJsonReader reader(doc, sizeof(doc) - 1);
        Y_ASSERT(reader.nextValue());
        Y_NO_THROW(reader.enter(), YsonException);
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_NO_THROW(reader.leave(), YsonException);
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextDocument());
    }

    void test_OptimizedObject()
    {
        char doc[] = "{$d#i\x03i\x03Key" "\x41\x8c\x00\x00" "i\x03Kay\x41\x8a\x00\x00"
                "i\x03Koy\x41\x89\x00\x00";
        UBJsonReader reader(doc, sizeof(doc) - 1);
        Y_ASSERT(reader.nextValue());
        Y_NO_THROW(reader.enter(), YsonException);
        Y_ASSERT(reader.nextKey());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(reader.nextKey());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(reader.nextKey());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(!reader.nextKey());
        Y_ASSERT(!reader.nextKey());
        Y_NO_THROW(reader.leave(), YsonException);
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextDocument());
    }

    Y_TEST(test_Basics,
           test_NextDocumentValue,
           test_Read,
           test_OptimizedArray,
           test_OptimizedObject);
}
