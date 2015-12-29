//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/JsonTokenizer.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void testNextToken(JsonTokenizer& tokenizer,
                       const std::string& input,
                       JsonTokenType_t expectedType,
                       const std::string& expectedToken)
    {
        tokenizer.setBuffer(&input[0], input.size());
        Y_ASSERT(tokenizer.hasNext());
        tokenizer.next();
        Y_EQUAL(tokenizer.tokenType(), expectedType);
        Y_EQUAL(tokenizer.token(), expectedToken);
    }

    void testNextToken(const std::string& input,
                       JsonTokenType_t expectedType,
                       const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                       const std::string& input2,
                       JsonTokenType_t expectedType,
                       const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, JsonTokenType::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                       const std::string& input2,
                       const std::string& input3,
                       JsonTokenType_t expectedType,
                       const std::string& expectedToken)
    {
        JsonTokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, JsonTokenType::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, JsonTokenType::END_OF_BUFFER, input1 + input2));
        Y_CALL(testNextToken(tokenizer, input3, expectedType, expectedToken));
    }

    void test_StringTokens()
    {
        Y_CALL(testNextToken("\"", JsonTokenType::END_OF_BUFFER, "\""));
        Y_CALL(testNextToken("\"\" ", JsonTokenType::STRING, "\"\""));
        Y_CALL(testNextToken("\"\n", JsonTokenType::INVALID_TOKEN, "\""));
        Y_CALL(testNextToken("\"\"\"", JsonTokenType::END_OF_BUFFER,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", JsonTokenType::END_OF_BUFFER,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", JsonTokenType::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"q", JsonTokenType::END_OF_BUFFER,
                             "\"\"\"\"\"q"));
        Y_CALL(testNextToken("\"\"\"\"\"\"", JsonTokenType::END_OF_BUFFER,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"q", JsonTokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"\"å", JsonTokenType::BLOCK_STRING,
                             "\"\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"", "", JsonTokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"", JsonTokenType::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", "", JsonTokenType::INVALID_TOKEN,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"", "", JsonTokenType::INVALID_TOKEN,
                             "\""));
        Y_CALL(testNextToken("\"\"", "", JsonTokenType::STRING,
                             "\"\""));
        Y_CALL(testNextToken("\"\"\"", "", JsonTokenType::INVALID_TOKEN,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"\"q", JsonTokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"", "\"\"", "", JsonTokenType::INVALID_TOKEN,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"", "\"abra\"\"\"\"", "",
                             JsonTokenType::BLOCK_STRING,
                             "\"\"\"\"abra\"\"\"\""));
        Y_CALL(testNextToken("\"\\", "\"\"", JsonTokenType::STRING,
                             "\"\\\"\""));
        Y_CALL(testNextToken("\"\\\\", "\"\"", JsonTokenType::STRING,
                             "\"\\\\\""));
        Y_CALL(testNextToken("\"\\\\\\", "\"\"", JsonTokenType::STRING,
                             "\"\\\\\\\"\""));
    }

    void test_NewlineTokens()
    {
        Y_CALL(testNextToken("\n", JsonTokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken("\n\r", JsonTokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken("\r", JsonTokenType::END_OF_BUFFER, "\r"));
        Y_CALL(testNextToken("\r", "", JsonTokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r\n", JsonTokenType::NEWLINE, "\r\n"));
        Y_CALL(testNextToken("\r\r", JsonTokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\ra", JsonTokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\r", JsonTokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\n", JsonTokenType::NEWLINE, "\r\n"));
    }

    void peek(const std::string& txt, JsonTokenType_t expectedToken,
              bool expectedCompleteToken)
    {
        JsonTokenizer tokenizer;
        tokenizer.setBuffer(txt.data(), txt.size());
        auto result = tokenizer.peek();
        Y_EQUAL(result.first, expectedToken);
        Y_EQUAL(result.second, expectedCompleteToken);
    }

    void test_Peek()
    {
        Y_CALL(peek("", JsonTokenType::END_OF_BUFFER, true));
        Y_CALL(peek("\"", JsonTokenType::STRING, false));
        Y_CALL(peek("0", JsonTokenType::VALUE, false));
        Y_CALL(peek(" ", JsonTokenType::WHITESPACE, false));
        Y_CALL(peek("0,", JsonTokenType::VALUE, true));
        Y_CALL(peek("\"\"", JsonTokenType::STRING, false));
        Y_CALL(peek("\"\" ", JsonTokenType::STRING, true));
        Y_CALL(peek(" 1", JsonTokenType::WHITESPACE, true));
    }

    void test_ValueTokens()
    {
        Y_CALL(testNextToken("8", JsonTokenType::END_OF_BUFFER, "8"));
        Y_CALL(testNextToken("---", JsonTokenType::END_OF_BUFFER, "---"));
        Y_CALL(testNextToken("---", "", JsonTokenType::VALUE, "---"));
        Y_CALL(testNextToken("--", "-", "", JsonTokenType::VALUE, "---"));
        Y_CALL(testNextToken("--", "---", "", JsonTokenType::VALUE, "-----"));
        Y_CALL(testNextToken("--", "---", "-- ", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--,", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--}", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--{ ", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--]", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--[ ", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--: ", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--\" ", JsonTokenType::VALUE, "-------"));
        Y_CALL(testNextToken("/a ", JsonTokenType::INVALID_TOKEN, "/a"));
        Y_CALL(testNextToken("/", "a", "", JsonTokenType::INVALID_TOKEN, "/a"));
        Y_CALL(testNextToken("8:", JsonTokenType::VALUE, "8"));
    }

    void test_CommentTokens()
    {
        Y_CALL(testNextToken("/", JsonTokenType::END_OF_BUFFER, "/"));
        Y_CALL(testNextToken("/", "", JsonTokenType::INVALID_TOKEN, "/"));
        Y_CALL(testNextToken("//", JsonTokenType::END_OF_BUFFER, "//"));
        Y_CALL(testNextToken("//", "", JsonTokenType::COMMENT, "//"));
        Y_CALL(testNextToken("//\n", JsonTokenType::COMMENT, "//"));
        Y_CALL(testNextToken("//\r", JsonTokenType::COMMENT, "//"));
        Y_CALL(testNextToken("// 12 { } [ ] \"\"\n", JsonTokenType::COMMENT,
                             "// 12 { } [ ] \"\""));

        Y_CALL(testNextToken("*/ ", JsonTokenType::VALUE, "*/"));
        Y_CALL(testNextToken("*/", "", JsonTokenType::VALUE, "*/"));
        Y_CALL(testNextToken("/* \n*/", JsonTokenType::BLOCK_COMMENT,
                             "/* \n*/"));
        Y_CALL(testNextToken("/* ", " * / */", JsonTokenType::BLOCK_COMMENT,
                             "/*  * / */"));
        Y_CALL(testNextToken("/* ", " /* // */", JsonTokenType::BLOCK_COMMENT,
                             "/*  /* // */"));
        Y_CALL(testNextToken("/* ", " /* // */*/", JsonTokenType::BLOCK_COMMENT,
                             "/*  /* // */"));
        Y_CALL(testNextToken("/* ", "", JsonTokenType::INVALID_TOKEN,
                             "/* "));
    }

    void test_WhitespaceTokens()
    {
        Y_CALL(testNextToken(" 1", JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken("\t1", JsonTokenType::WHITESPACE, "\t"));
        Y_CALL(testNextToken("\t 1", JsonTokenType::WHITESPACE, "\t "));
        Y_CALL(testNextToken(" \t 1", JsonTokenType::WHITESPACE, " \t "));
        Y_CALL(testNextToken(" ", "", JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken("\t", "", JsonTokenType::WHITESPACE, "\t"));
        Y_CALL(testNextToken("\t", " 1", JsonTokenType::WHITESPACE, "\t "));
        Y_CALL(testNextToken(" \t", " ", "", JsonTokenType::WHITESPACE, " \t "));
    }

    void testStructureToken(const std::string token,
                                   JsonTokenType_t expectedType)
    {
        Y_CALL(testNextToken(token, expectedType, token));
        Y_CALL(testNextToken(token + token, expectedType, token));
        Y_CALL(testNextToken(token + " ", expectedType, token));
        Y_CALL(testNextToken(token + "\"", expectedType, token));
    }

    void test_StructureTokens()
    {
        Y_CALL(testStructureToken("{", JsonTokenType::START_OBJECT));
        Y_CALL(testStructureToken("}", JsonTokenType::END_OBJECT));
        Y_CALL(testStructureToken("[", JsonTokenType::START_ARRAY));
        Y_CALL(testStructureToken("]", JsonTokenType::END_ARRAY));
        Y_CALL(testStructureToken(":", JsonTokenType::COLON));
        Y_CALL(testStructureToken(",", JsonTokenType::COMMA));
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
                       JsonTokenType_t type,
                       const std::string& token)
    {
        Y_ASSERT(tokenizer.hasNext());
        tokenizer.next();
        Y_EQUAL(tokenizer.tokenType(), type);
        Y_EQUAL(tokenizer.token(), token);
        auto range = tokenizer.rawToken();
        Y_EQUAL(std::string(range.first, range.second), token);
    }

    void test_SingleBuffer()
    {
        JsonTokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), json1.length());
        Y_CALL(testNextToken(tokenizer, JsonTokenType::START_OBJECT, "{"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::STRING,
                             "\"A value with\\n\\\"escaped characters\\\"\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::STRING, "\"list\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::START_ARRAY, "["));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::VALUE, "12"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::VALUE, "12.10"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::END_ARRAY, "]"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::NEWLINE, "\r\n"));
    }

    void test_MultiBuffer()
    {
        size_t splits[] = {28};
        JsonTokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), splits[0]);
        Y_CALL(testNextToken(tokenizer, JsonTokenType::START_OBJECT, "{"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, JsonTokenType::END_OF_BUFFER,
                             "\"A value with\\"));
        tokenizer.setBuffer(json1.c_str() + splits[0],
                            json1.size() - splits[0]);
        Y_CALL(testNextToken(tokenizer, JsonTokenType::STRING,
                             "\"A value with\\n\\\"escaped characters\\\"\""));
    }

    Y_TEST(test_StringTokens,
           test_NewlineTokens,
           test_CommentTokens,
           test_Peek,
           test_ValueTokens,
           test_WhitespaceTokens,
           test_StructureTokens,
           test_SingleBuffer,
           test_MultiBuffer);
}
