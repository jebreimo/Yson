//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/UBJsonReader.hpp"
#include "Ytest/Ytest.hpp"
#include "Yson/YsonException.hpp"
#include "Yson/Common/DefaultBufferSize.hpp"

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
    void readSucceeds(const char(&doc)[N], T expectedValue)
    {
        UBJsonReader reader(doc, sizeof(doc) - 1);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<T>(reader), expectedValue);
    }

    template <typename T>
    void readSucceeds(const std::string& doc, T expectedValue)
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
        Y_CALL(readSucceeds<int8_t>("i\x80", -128));
        Y_CALL(readSucceeds<int16_t>("i\x80", -128));
        Y_CALL(readFails<uint8_t>("i\x80"));
        Y_CALL(readFails<uint16_t>("i\x80"));

        Y_CALL(readSucceeds<uint8_t>("U\x7F", 127));
        Y_CALL(readSucceeds<int8_t>("U\x7F", 127));
        Y_CALL(readSucceeds<uint8_t>("U\x80", 128));
        Y_CALL(readSucceeds<int16_t>("U\x80", 128));
        Y_CALL(readSucceeds<uint32_t>("l\x20\x21\x22\x23", 0x20212223));
        Y_CALL(readFails<int8_t>("U\x80"));

        Y_CALL(readSucceeds<int8_t>("i\xFF", -1));
        Y_CALL(readFails<uint8_t>("i\xFF"));
        Y_CALL(readFails<uint64_t>("i\xFF"));

        Y_CALL(readSucceeds<float>("d\x41\x8c\x00\x00", 17.5));

        Y_CALL(readSucceeds<int16_t>("Si\x05""32767", 32767));
    }

    void test_OptimizedArray()
    {
        char doc[] = "[$d#I\x00\x03" "\x41\x8c\x00\x00" "\x41\x8a\x00\x00"
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

    void runScript(UBJsonReader& reader, const std::string& script)
    {
        for (auto c : script)
        {
            switch (c)
            {
            case 'v':
                Y_ASSERT(reader.nextValue());
                break;
            case 'k':
                Y_ASSERT(reader.nextKey());
                break;
            case '!':
                Y_ASSERT(!reader.nextValue());
                break;
            case '~':
                Y_ASSERT(!reader.nextKey());
                break;
            case 'e':
                Y_NO_THROW(reader.enter(), std::exception);
                break;
            case 'l':
                Y_NO_THROW(reader.leave(), std::exception);
                break;
            case 'd':
                Y_ASSERT(reader.nextDocument());
                break;
            case '^':
                Y_ASSERT(!reader.nextDocument());
                break;
            case 'A':
                Y_EQUAL(reader.valueType(), ValueType::ARRAY);
                break;
            case 'B':
                Y_EQUAL(reader.valueType(), ValueType::BOOLEAN);
                break;
            case 'I':
                Y_EQUAL(reader.valueType(), ValueType::INTEGER);
                break;
            case 'O':
                Y_EQUAL(reader.valueType(), ValueType::OBJECT);
                break;
            case 'S':
                Y_EQUAL(reader.valueType(), ValueType::STRING);
                break;
            case ' ':
                break;
            default:
                Y_FATAL_FAILURE("Test script contains unknown instruction!");
            }
        }
    }

    void runScript(std::pair<const char*, size_t> buffer,
                   const std::string& script)
    {
        UBJsonReader bufferReader(buffer.first, buffer.second);
        Y_CALL(runScript(bufferReader, script));
        std::stringstream ss(std::ios_base::binary | std::ios_base::in
                             | std::ios_base::out);
        ss.write(buffer.first, buffer.second);
        ss.seekg(0);
        UBJsonReader streamReader(ss);
        Y_CALL(runScript(streamReader, script));
    }

    void failingScript(std::pair<const char*, size_t> buffer,
                       const std::string& script)
    {
        UBJsonReader reader(buffer.first, buffer.second);
        Y_CALL(runScript(reader, script.substr(0, script.size() - 1)));
        switch (script.back())
        {
        case 'v':
            Y_ASSERT(!reader.nextValue());
            break;
        case 'k':
            Y_ASSERT(!reader.nextKey());
            break;
        case '!':
            Y_ASSERT(reader.nextValue());
            break;
        case 'e':
            Y_THROWS(reader.enter(), std::exception);
            break;
        case 'l':
            Y_THROWS(reader.leave(), std::exception);
            break;
        case 'd':
            Y_ASSERT(!reader.nextDocument());
            break;
        case '^':
            Y_ASSERT(reader.nextDocument());
            break;
        case 'A':
            Y_NOT_EQUAL(reader.valueType(), ValueType::ARRAY);
            break;
        case 'B':
            Y_NOT_EQUAL(reader.valueType(), ValueType::BOOLEAN);
            break;
        case 'I':
            Y_NOT_EQUAL(reader.valueType(), ValueType::INTEGER);
            break;
        case 'O':
            Y_NOT_EQUAL(reader.valueType(), ValueType::OBJECT);
            break;
        case 'S':
            Y_NOT_EQUAL(reader.valueType(), ValueType::STRING);
            break;
        case ' ':
            break;
        default:
            Y_FATAL_FAILURE("Test script contains unknown instruction!");
        }
    }

    template <size_t N>
    std::pair<const char*, size_t> S(const char (&values)[N])
    {
        return {values, N - 1};
    }

    void test_OptimizedObject()
    {
        const char doc[] = "{$d#i\x03i\x03Key"
                           "\x41\x8c\x00\x00"
                           "i\x03Kay\x41\x8a\x00\x00"
                           "i\x03Koy\x41\x89\x00\x00";
        Y_CALL(runScript(S(doc), "v ekvkvkv!!l !^"));
    }

    void test_SkipSubstructures()
    {
        Y_CALL(runScript(S("[{U\02**U\x03}]"), "v ev!l !^"));
        Y_CALL(runScript(S("{U\02**[U\x03[$U#U\x02\01\02]U\02##SU\x04%%%%}"), "v ekk~l !^"));
    }

    void test_MultiBufferValue()
    {
        auto globalBufferSize = getDefaultBufferSize();
        setDefaultBufferSize(10);
        try
        {
            auto buffer = S("SU\x14" "12345678901234567890");
            std::stringstream ss(std::ios_base::binary | std::ios_base::in
                                  | std::ios_base::out);
            ss.write(buffer.first, buffer.second);
            ss.seekg(0);
            UBJsonReader reader(ss);
            reader.nextValue();
            Y_EQUAL(Yson::read<std::string>(reader), buffer.first);
        }
        catch(...)
        {
            setDefaultBufferSize(globalBufferSize);
        }
    }

    Y_TEST(test_Basics,
           test_NextDocumentValue,
           test_Read,
           test_OptimizedArray,
           test_OptimizedObject,
           test_SkipSubstructures,
           test_MultiBufferValue);
}
