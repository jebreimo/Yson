//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonWriter.hpp"

#include <limits>
#include <sstream>

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        std::ostringstream stream;
        JsonWriter writer(stream, JsonFormatting::NONE);
        writer.key("Key");
        Y_EQUAL(writer.key(), "Key");
        writer.beginArray().value(12345678)
                .endArray().flush();
        Y_EQUAL(stream.str(), "[12345678]");
    }

    void test_EscapedString()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .value("\"foo\nbar\t\\x89baz\"").flush();
        std::string expected = R"("\"foo\nbar\t\\x89baz\"")";
        Y_EQUAL(ss.str(), expected);
    }

    void test_EscapedKey()
    {
        std::stringstream ss;
        JsonWriter(ss, Yson::JsonFormatting::FLAT)
            .beginObject()
            .key("\"\\").value(2)
            .endObject().flush();
        std::string expected = R"({"\"\\": 2})";
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
            .endArray().flush();

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

    void test_SemiManualFormatting()
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
            .endArray().flush();

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
            .endObject().flush();
        std::string expected =
                "{\n"
                "  \"name\": {}\n"
                "}";
        Y_EQUAL(ss.str(), expected);
    }


    void test_SimpleObject_NoStream()
    {
        JsonWriter writer(JsonFormatting::FORMAT);
        writer.beginObject()
            .key("name")
            .beginObject()
            .endObject()
            .endObject().flush();
        std::string expected =
                "{\n"
                "  \"name\": {}\n"
                "}";
        auto bufferAndSize = writer.buffer();
        std::string s(static_cast<const char*>(bufferAndSize.first),
                      bufferAndSize.second);
        Y_EQUAL(s, expected);
    }

    template <typename T>
    void doTestInteger(T value,
                      const std::string& expected)
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.value(value).flush();
        Y_EQUAL(ss.str(), expected);
    }

    void test_Integers()
    {
        Y_CALL(doTestInteger(32, "32"));
        Y_CALL(doTestInteger(char(20), "20"));
        Y_CALL(doTestInteger(static_cast<unsigned char>(20), "20"));
        Y_CALL(doTestInteger(static_cast<signed char>(20), "20"));
        Y_CALL(doTestInteger(static_cast<short>(20), "20"));
        Y_CALL(doTestInteger(static_cast<unsigned short>(20), "20"));
        Y_CALL(doTestInteger(static_cast<int>(20), "20"));
        Y_CALL(doTestInteger(static_cast<unsigned int>(20), "20"));
        Y_CALL(doTestInteger(static_cast<long>(20), "20"));
        Y_CALL(doTestInteger(static_cast<unsigned long>(20), "20"));
        Y_CALL(doTestInteger(static_cast<long long>(20), "20"));
        Y_CALL(doTestInteger(static_cast<unsigned long long>(20), "20"));
    }

    template <typename T>
    void doTestFloatingPoint(T value,
                             int precision,
                             const std::string& expected)
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setFloatingPointPrecision(precision);
        writer.value(value).flush();
        Y_EQUAL(ss.str(), expected);
    }

    void test_FloatingPointValues()
    {
        Y_CALL(doTestFloatingPoint(1.0f / 3.0f, 6, "0.333333"));
        Y_CALL(doTestFloatingPoint(1.0 / 3.0, 9, "0.333333333"));
    }

    void test_NonFiniteFloatingPointException()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        Y_THROWS(writer.value(std::numeric_limits<double>::infinity()),
                 std::runtime_error);
        writer.setNonFiniteFloatsEnabled(true);
        writer.value(std::numeric_limits<double>::infinity()).flush();
        Y_EQUAL(ss.str(), "Infinity");
    }

    void test_NonFiniteFloatingPointUnquoted()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setNonFiniteFloatsEnabled(true);
        writer.value(std::numeric_limits<double>::infinity()).flush();
        Y_EQUAL(ss.str(), "Infinity");
    }

    void test_NonFiniteFloatingPointQuoted()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setQuotedNonFiniteFloatsEnabled(true);
        writer.value(-std::numeric_limits<double>::infinity()).flush();
        Y_EQUAL(ss.str(), "\"-Infinity\"");
    }

    void test_UnquotedValueNames()
    {
        std::stringstream ss;
        JsonWriter writer(ss, JsonFormatting::NONE);
        writer.setUnquotedValueNamesEnabled(true);
        writer.beginObject()
              .key("Key1").value(0)
              .key("$Key2_").value(1)
              .key("_Key$3").value(2)
              .key("4Key4").value(3)
              .key("Key 5").value(4)
              .endObject().flush();
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
        writer.endObject().flush();
        Y_EQUAL(ss.str(),
                "{\n"
                "  \"1\": 2,\n"
                "  \"array\": [1, 2]\n"
                "}");
    }

    void test_SpecialValues()
    {
        std::stringstream ss;
        JsonWriter writer(ss, JsonFormatting::NONE);
        writer.beginArray();
        writer.boolean(true).boolean(false).null();
        writer.endArray().flush();
        Y_EQUAL(ss.str(), "[true,false,null]");
    }

    void test_MultiLineStrings()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setMultilineStringsEnabled(true).setMaximumLineWidth(8);
        writer.value("12345678901234567890").flush();
        Y_EQUAL(ss.str(),
                "\"123456\\\n"
                "7890123\\\n"
                "4567890\"");
    }

    void test_MultiLineStringsWithEscape()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setMultilineStringsEnabled(true).setMaximumLineWidth(8);
        writer.value("12345\"678901").flush();
        Y_EQUAL(ss.str(),
                "\"12345\\\n"
                "\\\"678901\"");
    }

    void test_EscapeNonAsciiCharacters()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.setEscapeNonAsciiCharactersEnabled(true);
        writer.value("ABC\"\360\220\220\267\"DEF").flush();
        Y_EQUAL(ss.str(), "\"ABC\\\"\\uD801\\uDC37\\\"DEF\"");
    }

    void test_LongWstring()
    {
        using namespace std::string_literals;
        std::stringstream ss;
        JsonWriter writer(ss);
        std::wstring ws = L"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name = D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name = A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak, min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer, Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område, Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223, Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)";
        writer.value(ws).flush();
        Y_EQUAL(ss.str(), reinterpret_cast<const char*>(u8"\"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name = D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name = A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak, min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer, Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område, Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223, Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)\""));
    }

    void test_IgnoreIncreasedFormatting()
    {
        std::ostringstream os;
        JsonWriter writer(os, JsonFormatting::NONE);
        writer.beginObject(JsonParameters(JsonFormatting::FLAT));
        writer.key("Name").value("Jan");
        writer.key("Id").value(200);
        writer.endObject().flush();
        Y_EQUAL(os.str(), "{\"Name\":\"Jan\",\"Id\":200}");
    }

    Y_TEST(test_Basics,
           test_EscapedString,
           test_EscapedKey,
           test_ManualFormatting,
           test_SemiManualFormatting,
           test_SimpleObject,
           test_SimpleObject_NoStream,
           test_Integers,
           test_FloatingPointValues,
           test_NonFiniteFloatingPointException,
           test_NonFiniteFloatingPointUnquoted,
           test_NonFiniteFloatingPointQuoted,
           test_UnquotedValueNames,
           test_FormatAndFlat,
           test_SpecialValues,
           test_MultiLineStrings,
           test_MultiLineStringsWithEscape,
           test_EscapeNonAsciiCharacters,
           test_LongWstring,
           test_IgnoreIncreasedFormatting);
}
