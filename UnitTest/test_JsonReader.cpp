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

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        char text[] = R"({"key": 123, "key2": "value"})";
        JsonReader reader(text, sizeof(text));

        Y_ASSERT(reader.nextValue());
        reader.enter();
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(read<std::string>(reader), "key");
        Y_EQUAL(reader.valueType(true), ValueType::STRING);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::STRING);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "123");
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(read<std::string>(reader), "key2");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "value");
        Y_ASSERT(!reader.nextKey());
        Y_ASSERT(!reader.nextKey());
        reader.leave();
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextDocument());
    }

    void doReadBase64(const std::string& str,
                      std::initializer_list<char> expected)
    {
        JsonReader reader(str.data(), str.size());
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.valueType(), ValueType::STRING);
        std::vector<char> value;
        Y_ASSERT(reader.readBase64(value));
        Y_EQUAL_RANGES(value, std::vector<char>(expected));
    }

    void test_read_base64()
    {
        Y_CALL(doReadBase64(R"("")", {}));
        Y_CALL(doReadBase64(R"("====")", {}));
        Y_CALL(doReadBase64(R"("A===")", {}));
        Y_CALL(doReadBase64(R"("AA==")", {0}));
        Y_CALL(doReadBase64(R"("AAA=")", {0, 0}));
        Y_CALL(doReadBase64(R"("AAAA")", {0, 0, 0}));
        Y_CALL(doReadBase64(R"("AAAAAA==")", {0, 0, 0, 0}));
        Y_CALL(doReadBase64(R"("AQ==")", {1}));
        Y_CALL(doReadBase64(R"("Bw==")", {7}));
    }

    void test_readNull()
    {
        char text[] = R"(null)";
        JsonReader reader(text, sizeof(text));
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(reader.readNull());
    }

    template <typename T>
    void assertRead(const std::string& text, T expectedValue)
    {
        JsonReader reader(text.data(), text.size());
        Y_ASSERT(reader.nextValue());
        T value;
        Y_ASSERT(reader.read(value));
        Y_EQUAL(value, expectedValue);
    }

    void test_EscapedString()
    {
        std::string doc = "\"AB\\\"\\n\\t\\/\\k\\\\g\\u0123\"";
        std::istringstream ss(doc);
        JsonReader reader(ss);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "AB\"\n\t/k\\g\xC4\xA3");
        Y_ASSERT(!reader.nextValue());
    }

    template <typename T>
    void assertRead(const std::string& text, T expectedValue, T epsilon)
    {
        JsonReader reader(text.data(), text.size());
        Y_ASSERT(reader.nextValue());
        T value;
        Y_ASSERT(reader.read(value));
        Y_EQUIVALENT(value, expectedValue, epsilon);
    }

    template <typename T>
    void assertReadFails(const std::string& text)
    {
        JsonReader reader(text.data(), text.size());
        Y_ASSERT(reader.nextValue());
        bool value;
        Y_ASSERT(!reader.read(value));
    }

    void test_read_integer()
    {
        Y_CALL(assertRead("true", true));
        Y_CALL(assertRead("false", false));
        Y_CALL(assertRead("0", false));
        Y_CALL(assertRead("1", true));
        Y_CALL(assertReadFails<bool>("falsd"));
        Y_CALL(assertReadFails<bool>("0.1"));
        Y_CALL(assertReadFails<bool>("2"));
        Y_CALL(assertReadFails<bool>("-1"));

        Y_CALL(assertRead<char>("-128", -128));
        Y_CALL(assertRead<char>("\"\\n\"", '\n'));

        Y_CALL(assertReadFails<int8_t>("-129"));
        Y_CALL(assertRead<int8_t>("01", 1));
        Y_CALL(assertRead<int8_t>("067", 67));
        Y_CALL(assertRead<int8_t>("000067", 67));
        Y_CALL(assertRead<int8_t>("-128", -128));
        Y_CALL(assertRead<int8_t>("1_00", 100));
        Y_CALL(assertRead<int8_t>("127", 127));
        Y_CALL(assertReadFails<int8_t>("128"));

        Y_CALL(assertReadFails<uint8_t>("-1"));
        Y_CALL(assertRead<uint8_t>("-0", 0));
        Y_CALL(assertRead<uint8_t>("1_00", 100));
        Y_CALL(assertRead<uint8_t>("255", 255));
        Y_CALL(assertReadFails<uint8_t>("256"));

        Y_CALL(assertReadFails<uint8_t>("-0b1"));
        Y_CALL(assertRead<uint8_t>("-0b0000", 0));
        Y_CALL(assertRead<uint8_t>("0b1", 1));
        Y_CALL(assertRead<uint8_t>("0b0000_0001", 1));
        Y_CALL(assertRead<uint8_t>("0b1111_1111", 255));
        Y_CALL(assertReadFails<uint8_t>("0b1_0000_0000"));

        Y_CALL(assertReadFails<int16_t>("-32769"));
        Y_CALL(assertRead<int16_t>("-32768", -32768));
        Y_CALL(assertRead<int16_t>("1_00", 100));
        Y_CALL(assertRead<int16_t>("32767", 32767));
        Y_CALL(assertReadFails<int16_t>("32768"));

        Y_CALL(assertReadFails<int64_t>("-0x8000_0000_0000_0001"));
        Y_CALL(assertRead<int64_t>("-0x8000_0000_0000_0000",
                                   -0x8000000000000000));
        Y_CALL(assertRead<int64_t>("1_00", 100));
        Y_CALL(assertRead<int64_t>("0x7FFF_FFFF_FFFF_FFFF",
                                   0x7FFFFFFFFFFFFFFF));
        Y_CALL(assertReadFails<int64_t>("0x8000_0000_0000_0000"));
    }

    void test_read_floating_point()
    {
        Y_CALL(assertRead<float>("100", 100.f));
        Y_CALL(assertRead<double>("100.123", 100.123));
        Y_CALL(assertRead<double>("-100.123", -100.123));
        Y_CALL(assertRead<double>("-100.123e15", -100.123e15));
        Y_CALL(assertRead<double>("-100.123e-15", -100.123e-15, 1e-25));
        Y_CALL(assertRead<double>("-100_100_100.100_100e+1_2",
                                  -100100100.100100e12));
        Y_CALL(assertRead<float>("1e38", 1e38f));
        Y_CALL(assertReadFails<float>("1e39"));
        Y_CALL(assertReadFails<double>("1e"));
        Y_CALL(assertReadFails<double>("1e+"));
        Y_CALL(assertReadFails<double>("1e-"));
        Y_CALL(assertReadFails<double>("1q"));
        Y_CALL(assertReadFails<double>("1_"));
        Y_CALL(assertReadFails<double>("1_.1"));
        Y_CALL(assertReadFails<double>("1._1"));
        Y_CALL(assertReadFails<double>("1.1_"));
        Y_CALL(assertReadFails<double>("1.1_e-1"));
        Y_CALL(assertReadFails<double>("1.1e_-1"));
        Y_CALL(assertReadFails<double>("1.1e-_1"));
        Y_CALL(assertReadFails<double>("1.1e-1_"));
        Y_CALL(assertRead<double>("1.1e-1", 1.1e-1));
        // TODO: test NaN, infinity and null
    }

    void test_LineAndColumnNumbers()
    {
        std::string doc = "{\n  \"foo\": 34, \"bar\": /* as df\ngh*/    64\n}";
        std::istringstream ss(doc);
        JsonReader reader(ss);
        Y_EQUAL(reader.lineNumber(), 1);
        Y_EQUAL(reader.columnNumber(), 1);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.lineNumber(), 1);
        Y_EQUAL(reader.columnNumber(), 2);
        Y_NO_THROW(reader.enter(), YsonException);
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(reader.lineNumber(), 2);
        Y_EQUAL(reader.columnNumber(), 8);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.lineNumber(), 2);
        Y_EQUAL(reader.columnNumber(), 12);
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(reader.lineNumber(), 2);
        Y_EQUAL(reader.columnNumber(), 19);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(reader.lineNumber(), 3);
        Y_EQUAL(reader.columnNumber(), 11);
        Y_NO_THROW(reader.leave(), YsonException);
        Y_EQUAL(reader.lineNumber(), 4);
        Y_EQUAL(reader.columnNumber(), 2);
    }

    void test_read_string()
    {
        Y_CALL(assertRead<std::string>(R"("100")", "100"));
        Y_CALL(assertRead<std::string>("\"100\\n\"", "100\n"));
        Y_CALL(assertRead<std::string>("\"100\\t200\\r\"", "100\t200\r"));
    }

    void test_read_single_quoted_string()
    {
        Y_CALL(assertRead<std::string>("'100'", "100"));
        Y_CALL(assertRead<std::string>("'100\\n'", "100\n"));
        Y_CALL(assertRead<std::string>("'100\\t200\\r'", "100\t200\r"));
        Y_CALL(assertRead<std::string>("'10\"0'", "10\"0"));
        Y_CALL(assertRead<std::string>("'10\\'0'", "10'0"));
    }

    void test_read_surrogate_pair()
    {
        Y_CALL(assertRead<std::string>("'\\uD83C\\uDFBC'", "\360\237\216\274"));
        Y_CALL(assertRead<std::string>("'\\uD83C'", "\355\240\274"));
        Y_CALL(assertRead<std::string>("'\\uD83CK'", "\355\240\274K"));
        Y_CALL(assertRead<std::string>("'\\uD83C\\''", "\355\240\274'"));
    }

    void test_read_multiline_string()
    {
        Y_CALL(assertRead<std::string>("'Line 1.\\\n Line 1 continued.'",
                                       "Line 1. Line 1 continued."));
        Y_CALL(assertRead<std::string>("'Line 1.\\\r\n Line 1 continued.'",
                                       "Line 1. Line 1 continued."));
    }

    void test_read_binary()
    {
        std::string doc = "\"SGVsbG8=\"";
        std::istringstream ss(doc);
        JsonReader reader(ss);
        Y_ASSERT(reader.nextValue());
        size_t size;
        Y_ASSERT(reader.readBinary(nullptr, size));
        Y_EQUAL(size, 5);
        std::string buffer(size, ' ');
        Y_ASSERT(reader.readBinary(&buffer[0], size));
        Y_EQUAL(size, 5);
        Y_EQUAL(buffer, "Hello");
    }

    void runScript(JsonReader& reader, const std::string& script)
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

    void runScript(const std::string& text, const std::string& script)
    {
        JsonReader reader(text.data(), text.size());
        Y_CALL(runScript(reader, script));
    }

    void failingScript(const std::string& text, const std::string& script)
    {
        JsonReader reader(text.data(), text.size());
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

    void test_array()
    {
        Y_CALL(runScript(R"(["a",2])", "ve vSvI! l!"));
        Y_CALL(runScript(R"([1, 2/*, 3*/])", "ve vIvI! l!"));
        Y_CALL(runScript(R"([1, 2/*, 3*/, 4])", "ve vIvIvI! l!"));
    }

    void test_document()
    {
        Y_CALL(runScript(R"({"a":2})", "d^"));
        Y_CALL(runScript(R"([{}] 1 "f")", "dvA dvI dvS ^"));
    }

    void test_object()
    {
        Y_CALL(runScript(R"({"a":2})", "vekSvI!l!^"));
        Y_CALL(runScript(R"({"a":2})", "vevI!l!^"));
        Y_CALL(runScript(R"({"a":2})", "vel!^"));
        Y_CALL(runScript(R"({"a":2})", "v!^"));
        Y_CALL(runScript(R"({"a":{"b":1,"c":2}})",
                         "ve kSve kSvIkSvI! l! l!^"));
        Y_CALL(failingScript(R"({"a":2})", "vekve"));
    }

    void test_object_with_unquoted_keys()
    {
        char text[] = R"({key: 123)";
        JsonReader reader(text, sizeof(text));

        Y_ASSERT(reader.nextValue());
        reader.enter();
        Y_ASSERT(reader.nextKey());
        Y_EQUAL(read<std::string>(reader), "key");
        Y_EQUAL(reader.valueType(true), ValueType::STRING);
        Y_EQUAL(reader.detailedValueType(true), DetailedValueType::STRING);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "123");
    }

    void test_non_ASCII_characters()
    {
        Y_CALL(runScript(R"("º")", "vS!^"));
    }

    void test_ValuesAsStrings()
    {
        std::string doc = "[null, 12.34, fooz, \"baz\"]";
        std::istringstream ss(doc);
        JsonReader reader(ss);
        //reader.setValuesAsStringsEnabled(true);
        Y_ASSERT(reader.nextValue());
        Y_NO_THROW(reader.enter(), YsonException);
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "null");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "12.34");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "fooz");
        Y_ASSERT(reader.nextValue());
        Y_EQUAL(read<std::string>(reader), "baz");
        Y_ASSERT(!reader.nextValue());
        Y_NO_THROW(reader.leave(), YsonException);
        Y_ASSERT(!reader.nextValue());
    }

    void test_end_of_document()
    {
        std::string text = "12 13";
        JsonReader reader(text.data(), text.size());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(reader.nextDocument());
        Y_ASSERT(reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextValue());
        Y_ASSERT(!reader.nextDocument());
    }

    Y_TEST(test_Basics,
           test_readNull,
           test_read_base64,
           test_read_floating_point,
           test_read_integer,
           test_read_string,
           test_read_single_quoted_string,
           test_read_surrogate_pair,
           test_read_multiline_string,
           test_read_binary,
           test_array,
           test_document,
           test_object,
           test_object_with_unquoted_keys,
           test_non_ASCII_characters,
           test_end_of_document,
           test_EscapedString,
           test_LineAndColumnNumbers,
           test_ValuesAsStrings);
}
