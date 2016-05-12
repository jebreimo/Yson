#include "../Yson/JsonWriter.hpp"

#include <limits>
#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_EscapedString()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .writeValue("\"foo\nbar\t\\x89baz\"");
        std::string expected = "\"\\\"foo\\nbar\\t\\\\x89baz\\\"\"";
        Y_EQUAL(ss.str(), expected);
    }

    void test_Newline()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .writeBeginArray(JsonWriter::FLAT)
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
        JsonWriter(ss)
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

    void test_FloatingPointValues()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        Y_THROWS(writer.writeValue(std::numeric_limits<double>::infinity()),
                 std::logic_error);
        writer.setNonFiniteFloatsAsStringsEnabled(true);
        writer.writeValue(std::numeric_limits<double>::infinity());
        Y_EQUAL(ss.str(), "\"infinity\"");
    }

    Y_TEST(test_EscapedString,
           test_Newline,
           test_SimpleObject,
           test_FloatingPointValues);
}
