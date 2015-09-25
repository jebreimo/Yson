#include "../Yson/JsonTokenizer.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void testNextToken(JsonTokenizer& tokenizer,
                              const std::string& input,
                              JsonTokenizer::TokenType expectedType,
                              const std::string& expectedToken)
    {
        tokenizer.setBuffer(&input[0], input.size());
        Y_ASSERT(tokenizer.hasNext());
        tokenizer.next();
        Y_EQUAL(tokenizer.tokenType(), expectedType);
        Y_EQUAL(tokenizer.token(), expectedToken);
    }

    void testNextToken(const std::string& input,
                              JsonTokenizer::TokenType expectedType,
                              const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                              const std::string& input2,
                              JsonTokenizer::TokenType expectedType,
                              const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, JsonTokenizer::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                              const std::string& input2,
                              const std::string& input3,
                              JsonTokenizer::TokenType expectedType,
                              const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, JsonTokenizer::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, JsonTokenizer::END_OF_BUFFER, input1 + input2));
        Y_CALL(testNextToken(tokenizer, input3, expectedType, expectedToken));
    }

    void test_StringTokens()
    {
        Y_CALL(testNextToken("\"", JsonTokenizer::END_OF_BUFFER, "\""));
        Y_CALL(testNextToken("\"\" ", JsonTokenizer::STRING, "\"\""));
        Y_CALL(testNextToken("\"\"\"", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"q", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\"\"\"q"));
        Y_CALL(testNextToken("\"\"\"\"\"\"", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"q", JsonTokenizer::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"\"å", JsonTokenizer::BLOCK_STRING,
                             "\"\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"", "", JsonTokenizer::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"", JsonTokenizer::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", "", JsonTokenizer::INVALID_TOKEN,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"", "", JsonTokenizer::INVALID_TOKEN,
                             "\""));
        Y_CALL(testNextToken("\"\"", "", JsonTokenizer::STRING,
                             "\"\""));
        Y_CALL(testNextToken("\"\"\"", "", JsonTokenizer::INVALID_TOKEN,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"\"q", JsonTokenizer::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"", "\"\"", "", JsonTokenizer::INVALID_TOKEN,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"", "\"abra\"\"\"\"", "",
                             JsonTokenizer::BLOCK_STRING,
                             "\"\"\"\"abra\"\"\"\""));
    }

    void test_NewlineTokens()
    {
        Y_CALL(testNextToken("\n", JsonTokenizer::NEWLINE, "\n"));
        Y_CALL(testNextToken("\n\r", JsonTokenizer::NEWLINE, "\n"));
        Y_CALL(testNextToken("\r", JsonTokenizer::END_OF_BUFFER, "\r"));
        Y_CALL(testNextToken("\r", "", JsonTokenizer::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r\n", JsonTokenizer::NEWLINE, "\r\n"));
        Y_CALL(testNextToken("\r\r", JsonTokenizer::NEWLINE, "\r"));
        Y_CALL(testNextToken("\ra", JsonTokenizer::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\r", JsonTokenizer::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\n", JsonTokenizer::NEWLINE, "\r\n"));
    }

    void test_ValueTokens()
    {
        Y_CALL(testNextToken("8", JsonTokenizer::END_OF_BUFFER, "8"));
        Y_CALL(testNextToken("---", JsonTokenizer::END_OF_BUFFER, "---"));
        Y_CALL(testNextToken("---", "", JsonTokenizer::VALUE, "---"));
        Y_CALL(testNextToken("--", "-", "", JsonTokenizer::VALUE, "---"));
        Y_CALL(testNextToken("--", "---", "", JsonTokenizer::VALUE, "-----"));
        Y_CALL(testNextToken("--", "---", "-- ", JsonTokenizer::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--,", JsonTokenizer::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--}", JsonTokenizer::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--{ ", JsonTokenizer::VALUE, "-------{"));
        Y_CALL(testNextToken("--", "---", "--]", JsonTokenizer::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--[ ", JsonTokenizer::VALUE, "-------["));
        Y_CALL(testNextToken("--", "---", "--: ", JsonTokenizer::VALUE, "-------:"));
        Y_CALL(testNextToken("--", "---", "--\" ", JsonTokenizer::VALUE, "-------\""));
        Y_CALL(testNextToken("/a ", JsonTokenizer::VALUE, "/a"));
    }

    void test_CommentTokens()
    {
        Y_CALL(testNextToken("/", JsonTokenizer::END_OF_BUFFER, "/"));
        Y_CALL(testNextToken("/", "", JsonTokenizer::VALUE, "/"));
        Y_CALL(testNextToken("//", JsonTokenizer::END_OF_BUFFER, "//"));
        Y_CALL(testNextToken("//", "", JsonTokenizer::COMMENT, "//"));
        Y_CALL(testNextToken("//\n", JsonTokenizer::COMMENT, "//"));
        Y_CALL(testNextToken("//\r", JsonTokenizer::COMMENT, "//"));
        Y_CALL(testNextToken("// 12 { } [ ] \"\"\n", JsonTokenizer::COMMENT, "// 12 { } [ ] \"\""));

        Y_CALL(testNextToken("*/ ", JsonTokenizer::VALUE, "*/"));
        Y_CALL(testNextToken("*/", "", JsonTokenizer::VALUE, "*/"));
        Y_CALL(testNextToken("/* \n*/", JsonTokenizer::BLOCK_COMMENT, "/* \n*/"));
        Y_CALL(testNextToken("/* ", " * / */", JsonTokenizer::BLOCK_COMMENT, "/*  * / */"));
        Y_CALL(testNextToken("/* ", " /* // */", JsonTokenizer::BLOCK_COMMENT, "/*  /* // */"));
        Y_CALL(testNextToken("/* ", " /* // */*/", JsonTokenizer::BLOCK_COMMENT, "/*  /* // */"));
        Y_CALL(testNextToken("/* ", "", JsonTokenizer::INVALID_TOKEN, "/* "));
    }

    void test_WhitespaceTokens()
    {
        Y_CALL(testNextToken(" ", JsonTokenizer::WHITESPACE, " "));
        Y_CALL(testNextToken("\t", JsonTokenizer::WHITESPACE, "\t"));
        Y_CALL(testNextToken("\t ", JsonTokenizer::WHITESPACE, "\t "));
        Y_CALL(testNextToken(" \t ", JsonTokenizer::WHITESPACE, " \t "));
    }

    void testStructureToken(const std::string token,
                                   JsonTokenizer::TokenType expectedType)
    {
        Y_CALL(testNextToken(token, expectedType, token));
        Y_CALL(testNextToken(token + token, expectedType, token));
        Y_CALL(testNextToken(token + " ", expectedType, token));
        Y_CALL(testNextToken(token + "\"", expectedType, token));
    }

    void test_StructureTokens()
    {
        Y_CALL(testStructureToken("{", JsonTokenizer::OBJECT_START));
        Y_CALL(testStructureToken("}", JsonTokenizer::OBJECT_END));
        Y_CALL(testStructureToken("[", JsonTokenizer::ARRAY_START));
        Y_CALL(testStructureToken("]", JsonTokenizer::ARRAY_END));
        Y_CALL(testStructureToken(":", JsonTokenizer::COLON));
        Y_CALL(testStructureToken(",", JsonTokenizer::COMMA));
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
        Y_ASSERT(tokenizer.hasNext());
        tokenizer.next();
        Y_EQUAL(tokenizer.tokenType(), type);
        Y_EQUAL(tokenizer.token(), token);
        // Y_EQUAL(tokenizer.lineNo(), lineNo);
        // Y_EQUAL(tokenizer.columnNo(), colNo);
        const char *start, *end;
        tokenizer.getToken(start, end);
        Y_EQUAL(std::string(start, end), token);
    }

    void test_SingleBuffer()
    {
        JsonTokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), json1.length());
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::OBJECT_START, "{"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::STRING,
                            "\"A value with\\n\\\"escaped characters\\\"\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::STRING, "\"list\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::ARRAY_START, "["));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::VALUE, "12"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::VALUE, "12.10"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::ARRAY_END, "]"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::NEWLINE, "\r\n"));
    }

    void test_MultiBuffer()
    {
        size_t splits[] = {28};
        JsonTokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), splits[0]);
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::OBJECT_START, "{"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::END_OF_BUFFER,
                             "\"A value with\\"));
        tokenizer.setBuffer(json1.c_str() + splits[0], json1.size() - splits[0]);
        Y_CALL(testNextToken(tokenizer, JsonTokenizer::STRING,
                             "\"A value with\\n\\\"escaped characters\\\"\""));
    }

    Y_TEST(test_StringTokens,
           test_NewlineTokens,
           test_CommentTokens,
           test_ValueTokens,
           test_WhitespaceTokens,
           test_StructureTokens,
           test_SingleBuffer,
           test_MultiBuffer);
}
