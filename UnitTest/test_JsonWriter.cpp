//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/JsonWriter.hpp"

#include <limits>
#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        std::ostringstream stream;
        JsonWriter writer(stream);
        writer.key("Key");
        Y_EQUAL(writer.key(), "Key");
        writer.beginArray().value(12345678)
                .endArray();
        Y_EQUAL(stream.str(), "[12345678]");
    }

    void test_EscapedString()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .value("\"foo\nbar\t\\x89baz\"");
        std::string expected = "\"\\\"foo\\nbar\\t\\\\x89baz\\\"\"";
        Y_EQUAL(ss.str(), expected);
    }

    void test_ManualFormatting()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .beginArray(JsonParameters(JsonFormatting::FLAT))
            .writeNewline()
            .indent()

            .writeIndentation()
            .rawText("// A 4x4 sudoku")
            .writeNewline()

            .writeIndentation()
            .value(1).value(2).writeComma().writeSeparator(2)
            .value(3).value(4).writeComma().writeNewline()

            .writeIndentation()
            .value(3).value(4).writeComma().writeSeparator(2)
            .value(1).value(2).writeComma().writeNewline()

            .writeNewline()

            .writeIndentation()
            .value(2).value(1).writeComma().writeSeparator(2)
            .value(4).value(3).writeComma().writeNewline()

            .writeIndentation()
            .value(4).value(3).writeComma().writeSeparator(2)
            .value(2).value(1).writeNewline()

            .outdent()
            .endArray();

        std::string expected =
                "[\n"
                "  // A 4x4 sudoku\n"
                "  1, 2,  3, 4,\n"
                "  3, 4,  1, 2,\n"
                "\n"
                "  2, 1,  4, 3,\n"
                "  4, 3,  2, 1\n"
                "]";
        Y_EQUAL(ss.str(), expected);
    }

    void test_MultiValueLines()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .beginArray(JsonParameters(4))
            .writeNewline().writeIndentation()
            .rawText("// A 4x4 sudoku").writeNewline()
            .value(1).value(2).writeComma().writeSeparator(2)
            .value(3).value(4)
            .value(3).value(4).writeComma().writeSeparator(2)
            .value(1).value(2)
            .writeComma().writeNewline().writeNewline()
            .value(2).value(1).writeComma().writeSeparator(2)
            .value(4).value(3)
            .value(4).value(3).writeComma().writeSeparator(2)
            .value(2).value(1)
            .endArray();

        std::string expected =
                "[\n"
                "  // A 4x4 sudoku\n"
                "  1, 2,  3, 4,\n"
                "  3, 4,  1, 2,\n"
                "\n"
                "  2, 1,  4, 3,\n"
                "  4, 3,  2, 1\n"
                "]";
        Y_EQUAL(ss.str(), expected);
    }

    void test_SimpleObject()
    {
        std::stringstream ss;
        JsonWriter(ss, JsonFormatting::FORMAT)
            .beginObject()
            .key("name")
            .beginObject()
            .endObject()
            .endObject();
        std::string expected =
                "{\n"
                "  \"name\": {}\n"
                "}";
        Y_EQUAL(ss.str(), expected);
    }

    template <typename T>
    void doTestInteger(T value,
                      const std::string& expected)
    {
       std::stringstream ss;
       JsonWriter writer(ss);
       writer.value(value);
       Y_EQUAL(ss.str(), expected);
    }

    void test_Integers()
    {
       doTestInteger(32, "32");
       doTestInteger(char(20), "20");
    }

    void test_FloatingPointValues()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        Y_THROWS(writer.value(std::numeric_limits<double>::infinity()),
                 std::logic_error);
        writer.setNonFiniteFloatsAsStringsEnabled(true);
        writer.value(std::numeric_limits<double>::infinity());
        Y_EQUAL(ss.str(), "\"infinity\"");
    }

    void test_UnquotedValueNames()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setUnquotedValueNamesEnabled(true);
        writer.beginObject()
              .key("Key1").value(0)
              .key("$Key2_").value(1)
              .key("_Key$3").value(2)
              .key("4Key4").value(3)
              .key("Key 5").value(4)
              .endObject();
        Y_EQUAL(ss.str(),
                "{Key1:0,$Key2_:1,_Key$3:2,\"4Key4\":3,\"Key 5\":4}");
    }

    void test_FormatAndFlat()
    {
        std::stringstream ss;
        JsonWriter writer(ss, JsonFormatting::FORMAT);
        writer.beginObject();
        writer.key("1").value(2);
        writer.key("array").beginArray(JsonParameters(JsonFormatting::FLAT)).value(1).value(2).endArray();
        writer.endObject();
        Y_EQUAL(ss.str(),
                "{\n"
                "  \"1\": 2,\n"
                "  \"array\": [1, 2]\n"
                "}");
    }

    Y_TEST(test_Basics,
           test_EscapedString,
           test_ManualFormatting,
           test_MultiValueLines,
           test_SimpleObject,
           test_Integers,
           test_FloatingPointValues,
           test_UnquotedValueNames,
           test_FormatAndFlat);
}
