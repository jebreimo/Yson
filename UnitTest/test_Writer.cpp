#include "../Yson/Writer.hpp"

#include <limits>
#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_EscapedString()
    {
        std::stringstream ss;
        Writer(ss)
            .writeValue("\"foo\nbar\t\\x89baz\"");
        std::string expected = "\"\\\"foo\\nbar\\t\\\\x89baz\\\"\"";
        Y_EQUAL(ss.str(), expected);
    }

    void test_Newline()
    {
        std::stringstream ss;
        Writer(ss)
            .writeBeginArray(Writer::FLAT)
            .writeNewline()
            .indent()

            .writeIndentation()
            .writeRawText("// A 4x4 sudoku")
            .writeNewline()

            .writeIndentation()
            .writeValue(1).writeValue(2).writeComma().writeSeparator(2)
            .writeValue(3).writeValue(4).writeComma().writeNewline()

            .writeIndentation()
            .writeValue(3).writeValue(4).writeComma().writeSeparator(2)
            .writeValue(1).writeValue(2).writeComma().writeNewline()

            .writeNewline()

            .writeIndentation()
            .writeValue(2).writeValue(1).writeComma().writeSeparator(2)
            .writeValue(4).writeValue(3).writeComma().writeNewline()

            .writeIndentation()
            .writeValue(4).writeValue(3).writeComma().writeSeparator(2)
            .writeValue(2).writeValue(1).writeNewline()

            .outdent()
            .writeEndArray();

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
        Writer(ss)
            .writeBeginObject()
            .setValueName("name")
            .writeBeginObject()
            .writeEndObject()
            .writeEndObject();
        std::string expected =
                "{\n"
                "  \"name\": {}\n"
                "}";
        Y_EQUAL(ss.str(), expected);
    }

    template <typename T>
    void doTestInteger(T value,
                       Writer::IntegerMode mode,
                       const std::string& expected)
    {
        std::stringstream ss;
        Writer writer(ss);
        writer.setIntegerMode(mode);
        writer.writeValue(value);
        Y_EQUAL(ss.str(), expected);
    }

    void test_Integers()
    {
        doTestInteger(32, Writer::HEXADECIMAL, R"("0x20")");
        doTestInteger(20, Writer::BINARY, R"("0b10100")");
        doTestInteger(20, Writer::OCTAL, R"("0o24")");
    }

    void test_FloatingPointValues()
    {
        std::stringstream ss;
        Writer writer(ss);
        Y_THROWS(writer.writeValue(std::numeric_limits<double>::infinity()),
                 std::logic_error);
        writer.setNonFiniteFloatsAsStringsEnabled(true);
        writer.writeValue(std::numeric_limits<double>::infinity());
        Y_EQUAL(ss.str(), "\"infinity\"");
    }

    void test_UnquotedValueNames()
    {
        std::stringstream ss;
        Writer writer(ss);
        writer.setUnquotedValueNamesEnabled(true).setFormattingEnabled(false);
        writer.writeBeginObject()
                .writeValue("Key1", 0)
                .writeValue("$Key2_", 1)
                .writeValue("_Key$3", 2)
                .writeValue("4Key4", 3)
                .writeValue("Key 5", 4)
                .writeEndObject();
        Y_EQUAL(ss.str(),
                "{Key1:0,$Key2_:1,_Key$3:2,\"4Key4\":3,\"Key 5\":4}");
    }

    Y_TEST(test_EscapedString,
           test_Newline,
           test_SimpleObject,
           test_Integers,
           test_FloatingPointValues,
           test_UnquotedValueNames);
}
