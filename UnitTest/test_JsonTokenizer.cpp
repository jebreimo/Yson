#include "JEBIO/Json/JsonTokenizer.hpp"

#include <JEBTest/JEBTest.hpp>

namespace
{
using namespace JEBIO::Json;

void testNextToken(JsonTokenizer& tokenizer,
                          const std::string& input,
                          JsonTokenizer::TokenType expectedType,
                          const std::string& expectedToken)
{
    tokenizer.setBuffer(&input[0], input.size());
    JT_ASSERT(tokenizer.hasNext());
    tokenizer.next();
    JT_EQUAL(tokenizer.tokenType(), expectedType);
    JT_EQUAL(tokenizer.token(), expectedToken);
}

void testNextToken(const std::string& input,
                          JsonTokenizer::TokenType expectedType,
                          const std::string& expectedToken)
{
    JsonTokenizer tokenizer;
    JT_CALL(testNextToken(tokenizer, input, expectedType, expectedToken));
}

void testNextToken(const std::string& input1,
                          const std::string& input2,
                          JsonTokenizer::TokenType expectedType,
                          const std::string& expectedToken)
{
    JsonTokenizer tokenizer;
    JT_CALL(testNextToken(tokenizer, input1, JsonTokenizer::EndOfBuffer, input1));
    JT_CALL(testNextToken(tokenizer, input2, expectedType, expectedToken));
}

void testNextToken(const std::string& input1,
                          const std::string& input2,
                          const std::string& input3,
                          JsonTokenizer::TokenType expectedType,
                          const std::string& expectedToken)
{
    JsonTokenizer tokenizer;
    JT_CALL(testNextToken(tokenizer, input1, JsonTokenizer::EndOfBuffer, input1));
    JT_CALL(testNextToken(tokenizer, input2, JsonTokenizer::EndOfBuffer, input1 + input2));
    JT_CALL(testNextToken(tokenizer, input3, expectedType, expectedToken));
}

void test_StringTokens()
{
    JT_CALL(testNextToken("\"", JsonTokenizer::EndOfBuffer, "\""));
    JT_CALL(testNextToken("\"\" ", JsonTokenizer::String, "\"\""));
    JT_CALL(testNextToken("\"\"\"", JsonTokenizer::EndOfBuffer,
                          "\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"", JsonTokenizer::EndOfBuffer,
                          "\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"", JsonTokenizer::EndOfBuffer,
                          "\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"q", JsonTokenizer::EndOfBuffer,
                          "\"\"\"\"\"q"));
    JT_CALL(testNextToken("\"\"\"\"\"\"", JsonTokenizer::EndOfBuffer,
                          "\"\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"\"q", JsonTokenizer::BlockString,
                          "\"\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"\"\"å", JsonTokenizer::BlockString,
                          "\"\"\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"\"", "", JsonTokenizer::BlockString,
                          "\"\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"", "\"", JsonTokenizer::EndOfBuffer,
                          "\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"\"", "", JsonTokenizer::InvalidToken,
                          "\"\"\"\"\""));
    JT_CALL(testNextToken("\"", "", JsonTokenizer::InvalidToken,
                          "\""));
    JT_CALL(testNextToken("\"\"", "", JsonTokenizer::String,
                          "\"\""));
    JT_CALL(testNextToken("\"\"\"", "", JsonTokenizer::InvalidToken,
                          "\"\"\""));
    JT_CALL(testNextToken("\"\"\"\"", "\"\"q", JsonTokenizer::BlockString,
                          "\"\"\"\"\"\""));
    JT_CALL(testNextToken("\"\"", "\"\"", "", JsonTokenizer::InvalidToken,
                          "\"\"\"\""));
    JT_CALL(testNextToken("\"\"\"", "\"abra\"\"\"\"", "",
                          JsonTokenizer::BlockString,
                          "\"\"\"\"abra\"\"\"\""));
}

void test_NewlineTokens()
{
    JT_CALL(testNextToken("\n", JsonTokenizer::Newline, "\n"));
    JT_CALL(testNextToken("\n\r", JsonTokenizer::Newline, "\n"));
    JT_CALL(testNextToken("\r", JsonTokenizer::EndOfBuffer, "\r"));
    JT_CALL(testNextToken("\r", "", JsonTokenizer::Newline, "\r"));
    JT_CALL(testNextToken("\r\n", JsonTokenizer::Newline, "\r\n"));
    JT_CALL(testNextToken("\r\r", JsonTokenizer::Newline, "\r"));
    JT_CALL(testNextToken("\ra", JsonTokenizer::Newline, "\r"));
    JT_CALL(testNextToken("\r", "\r", JsonTokenizer::Newline, "\r"));
    JT_CALL(testNextToken("\r", "\n", JsonTokenizer::Newline, "\r\n"));
}

void test_ValueTokens()
{
    JT_CALL(testNextToken("8", JsonTokenizer::EndOfBuffer, "8"));
    JT_CALL(testNextToken("---", JsonTokenizer::EndOfBuffer, "---"));
    JT_CALL(testNextToken("---", "", JsonTokenizer::Value, "---"));
    JT_CALL(testNextToken("--", "-", "", JsonTokenizer::Value, "---"));
    JT_CALL(testNextToken("--", "---", "", JsonTokenizer::Value, "-----"));
    JT_CALL(testNextToken("--", "---", "-- ", JsonTokenizer::Value, "-------"));
    JT_CALL(testNextToken("--", "---", "--,", JsonTokenizer::Value, "-------"));
    JT_CALL(testNextToken("--", "---", "--}", JsonTokenizer::Value, "-------"));
    JT_CALL(testNextToken("--", "---", "--{ ", JsonTokenizer::Value, "-------{"));
    JT_CALL(testNextToken("--", "---", "--]", JsonTokenizer::Value, "-------"));
    JT_CALL(testNextToken("--", "---", "--[ ", JsonTokenizer::Value, "-------["));
    JT_CALL(testNextToken("--", "---", "--: ", JsonTokenizer::Value, "-------:"));
    JT_CALL(testNextToken("--", "---", "--\" ", JsonTokenizer::Value, "-------\""));
    JT_CALL(testNextToken("/a ", JsonTokenizer::Value, "/a"));
}

void test_CommentTokens()
{
    JT_CALL(testNextToken("/", JsonTokenizer::EndOfBuffer, "/"));
    JT_CALL(testNextToken("/", "", JsonTokenizer::Value, "/"));
    JT_CALL(testNextToken("//", JsonTokenizer::EndOfBuffer, "//"));
    JT_CALL(testNextToken("//", "", JsonTokenizer::Comment, "//"));
    JT_CALL(testNextToken("//\n", JsonTokenizer::Comment, "//"));
    JT_CALL(testNextToken("//\r", JsonTokenizer::Comment, "//"));
    JT_CALL(testNextToken("// 12 { } [ ] \"\"\n", JsonTokenizer::Comment, "// 12 { } [ ] \"\""));

    JT_CALL(testNextToken("*/ ", JsonTokenizer::Value, "*/"));
    JT_CALL(testNextToken("*/", "", JsonTokenizer::Value, "*/"));
    JT_CALL(testNextToken("/* \n*/", JsonTokenizer::BlockComment, "/* \n*/"));
    JT_CALL(testNextToken("/* ", " * / */", JsonTokenizer::BlockComment, "/*  * / */"));
    JT_CALL(testNextToken("/* ", " /* // */", JsonTokenizer::BlockComment, "/*  /* // */"));
    JT_CALL(testNextToken("/* ", " /* // */*/", JsonTokenizer::BlockComment, "/*  /* // */"));
    JT_CALL(testNextToken("/* ", "", JsonTokenizer::InvalidToken, "/* "));
}

void test_WhitespaceTokens()
{
    JT_CALL(testNextToken(" ", JsonTokenizer::Whitespace, " "));
    JT_CALL(testNextToken("\t", JsonTokenizer::Whitespace, "\t"));
    JT_CALL(testNextToken("\t ", JsonTokenizer::Whitespace, "\t "));
    JT_CALL(testNextToken(" \t ", JsonTokenizer::Whitespace, " \t "));
}

void testStructureToken(const std::string token,
                               JsonTokenizer::TokenType expectedType)
{
    JT_CALL(testNextToken(token, expectedType, token));
    JT_CALL(testNextToken(token + token, expectedType, token));
    JT_CALL(testNextToken(token + " ", expectedType, token));
    JT_CALL(testNextToken(token + "\"", expectedType, token));
}

void test_StructureTokens()
{
    JT_CALL(testStructureToken("{", JsonTokenizer::ObjectStart));
    JT_CALL(testStructureToken("}", JsonTokenizer::ObjectEnd));
    JT_CALL(testStructureToken("[", JsonTokenizer::ArrayStart));
    JT_CALL(testStructureToken("]", JsonTokenizer::ArrayEnd));
    JT_CALL(testStructureToken(":", JsonTokenizer::Colon));
    JT_CALL(testStructureToken(",", JsonTokenizer::Comma));
}

std::string json1 = "\
{\n\
    \"text\": \"A value with\\n\\\"escaped characters\\\"\",\n\
    \"list\": [12, 12.10],\r\n\
    \"true bool\": true,\n\
    \"false bool\": false,\n\
    \"null\": null\n\
}";

void testNextToken(JsonTokenizer& tokenizer,
                   JsonTokenizer::TokenType type,
                   const std::string& token)
{
    JT_ASSERT(tokenizer.hasNext());
    tokenizer.next();
    JT_EQUAL(tokenizer.tokenType(), type);
    JT_EQUAL(tokenizer.token(), token);
    // JT_EQUAL(tokenizer.lineNo(), lineNo);
    // JT_EQUAL(tokenizer.columnNo(), colNo);
    const char *start, *end;
    tokenizer.getToken(start, end);
    JT_EQUAL(std::string(start, end), token);
}

void test_SingleBuffer()
{
    JsonTokenizer tokenizer;
    tokenizer.setBuffer(json1.c_str(), json1.length());
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::ObjectStart, "{"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Newline, "\n"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, "    "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::String, "\"text\""));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Colon, ":"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, " "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::String,
                          "\"A value with\\n\\\"escaped characters\\\"\""));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Comma, ","));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Newline, "\n"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, "    "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::String, "\"list\""));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Colon, ":"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, " "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::ArrayStart, "["));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Value, "12"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Comma, ","));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, " "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Value, "12.10"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::ArrayEnd, "]"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Comma, ","));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Newline, "\r\n"));
}

void test_MultiBuffer()
{
    size_t splits[] = {28};
    JsonTokenizer tokenizer;
    tokenizer.setBuffer(json1.c_str(), splits[0]);
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::ObjectStart, "{"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Newline, "\n"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, "    "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::String, "\"text\""));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Colon, ":"));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::Whitespace, " "));
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::EndOfBuffer,
                          "\"A value with\\"));
    tokenizer.setBuffer(json1.c_str() + splits[0], json1.size() - splits[0]);
    JT_CALL(testNextToken(tokenizer, JsonTokenizer::String,
                          "\"A value with\\n\\\"escaped characters\\\"\""));
}

JT_TEST(test_StringTokens,
        test_NewlineTokens,
        test_CommentTokens,
        test_ValueTokens,
        test_WhitespaceTokens,
        test_StructureTokens,
        test_SingleBuffer,
        test_MultiBuffer);
}
