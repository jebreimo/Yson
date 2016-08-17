//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/Tokenizer.hpp"

#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void testNextToken(Tokenizer& tokenizer,
                       const std::string& input,
                       TokenType expectedType,
                       const std::string& expectedToken)
    {
        tokenizer.setBuffer(&input[0], input.size());
        Y_ASSERT(tokenizer.hasNext());
        tokenizer.next();
        Y_EQUAL(tokenizer.tokenType(), expectedType);
        Y_EQUAL(tokenizer.token(), expectedToken);
    }

    void testNextToken(const std::string& input,
                       TokenType expectedType,
                       const std::string& expectedToken)
    {
        Tokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                       const std::string& input2,
                       TokenType expectedType,
                       const std::string& expectedToken)
    {
        Tokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, TokenType::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, expectedType, expectedToken));
    }

    void testNextToken(const std::string& input1,
                       const std::string& input2,
                       const std::string& input3,
                       TokenType expectedType,
                       const std::string& expectedToken)
    {
        Tokenizer tokenizer;
        Y_CALL(testNextToken(tokenizer, input1, TokenType::END_OF_BUFFER, input1));
        Y_CALL(testNextToken(tokenizer, input2, TokenType::END_OF_BUFFER, input1 + input2));
        Y_CALL(testNextToken(tokenizer, input3, expectedType, expectedToken));
    }

    void test_StringTokens()
    {
        Y_CALL(testNextToken("\"", TokenType::END_OF_BUFFER, "\""));
        Y_CALL(testNextToken("\"\" ", TokenType::STRING, "\"\""));
        Y_CALL(testNextToken("\"\n", TokenType::INVALID_TOKEN, "\""));
        Y_CALL(testNextToken("\"\"\"", TokenType::END_OF_BUFFER,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", TokenType::END_OF_BUFFER,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", TokenType::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"q", TokenType::END_OF_BUFFER,
                             "\"\"\"\"\"q"));
        Y_CALL(testNextToken("\"\"\"\"\"\"", TokenType::END_OF_BUFFER,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"q", TokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"\"å", TokenType::BLOCK_STRING,
                             "\"\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"\"", "", TokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"", TokenType::END_OF_BUFFER,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"\"", "", TokenType::INVALID_TOKEN,
                             "\"\"\"\"\""));
        Y_CALL(testNextToken("\"", "", TokenType::INVALID_TOKEN,
                             "\""));
        Y_CALL(testNextToken("\"\"", "", TokenType::STRING,
                             "\"\""));
        Y_CALL(testNextToken("\"\"\"", "", TokenType::INVALID_TOKEN,
                             "\"\"\""));
        Y_CALL(testNextToken("\"\"\"\"", "\"\"q", TokenType::BLOCK_STRING,
                             "\"\"\"\"\"\""));
        Y_CALL(testNextToken("\"\"", "\"\"", "", TokenType::INVALID_TOKEN,
                             "\"\"\"\""));
        Y_CALL(testNextToken("\"\"\"", "\"abra\"\"\"\"", "",
                             TokenType::BLOCK_STRING,
                             "\"\"\"\"abra\"\"\"\""));
        Y_CALL(testNextToken("\"\\", "\"\"", TokenType::STRING,
                             "\"\\\"\""));
        Y_CALL(testNextToken("\"\\\\", "\"\"", TokenType::STRING,
                             "\"\\\\\""));
        Y_CALL(testNextToken("\"\\\\\\", "\"\"", TokenType::STRING,
                             "\"\\\\\\\"\""));
    }

    void test_NewlineTokens()
    {
        Y_CALL(testNextToken("\n", TokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken("\n\r", TokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken("\r", TokenType::END_OF_BUFFER, "\r"));
        Y_CALL(testNextToken("\r", "", TokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r\n", TokenType::NEWLINE, "\r\n"));
        Y_CALL(testNextToken("\r\r", TokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\ra", TokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\r", TokenType::NEWLINE, "\r"));
        Y_CALL(testNextToken("\r", "\n", TokenType::NEWLINE, "\r\n"));
    }

    void peek(const std::string& txt, TokenType expectedToken,
              bool expectedCompleteToken)
    {
        Tokenizer tokenizer;
        tokenizer.setBuffer(txt.data(), txt.size());
        auto result = tokenizer.peek();
        Y_EQUAL(result.first, expectedToken);
        Y_EQUAL(result.second, expectedCompleteToken);
    }

    void test_Peek()
    {
        Y_CALL(peek("", TokenType::END_OF_BUFFER, true));
        Y_CALL(peek("\"", TokenType::STRING, false));
        Y_CALL(peek("0", TokenType::VALUE, false));
        Y_CALL(peek(" ", TokenType::WHITESPACE, false));
        Y_CALL(peek("0,", TokenType::VALUE, true));
        Y_CALL(peek("\"\"", TokenType::STRING, false));
        Y_CALL(peek("\"\" ", TokenType::STRING, true));
        Y_CALL(peek(" 1", TokenType::WHITESPACE, true));

        std::string txt = "1 2";
        Tokenizer tokenizer;
        tokenizer.setBuffer(txt.data(), txt.size());
        Y_EQUAL(tokenizer.peek(), std::make_pair(TokenType::VALUE, true));
        tokenizer.next();
        Y_EQUAL(tokenizer.peek(),
                std::make_pair(TokenType::WHITESPACE, true));
        tokenizer.next();
        Y_EQUAL(tokenizer.peek(), std::make_pair(TokenType::VALUE, false));
    }

    void test_ValueTokens()
    {
        Y_CALL(testNextToken("8", TokenType::END_OF_BUFFER, "8"));
        Y_CALL(testNextToken("---", TokenType::END_OF_BUFFER, "---"));
        Y_CALL(testNextToken("---", "", TokenType::VALUE, "---"));
        Y_CALL(testNextToken("--", "-", "", TokenType::VALUE, "---"));
        Y_CALL(testNextToken("--", "---", "", TokenType::VALUE, "-----"));
        Y_CALL(testNextToken("--", "---", "-- ", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--,", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--}", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--{ ", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--]", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--[ ", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--: ", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("--", "---", "--\" ", TokenType::VALUE, "-------"));
        Y_CALL(testNextToken("/a ", TokenType::INVALID_TOKEN, "/a"));
        Y_CALL(testNextToken("/", "a", "", TokenType::INVALID_TOKEN, "/a"));
        Y_CALL(testNextToken("8:", TokenType::VALUE, "8"));
    }

    void test_CommentTokens()
    {
        Y_CALL(testNextToken("/", TokenType::END_OF_BUFFER, "/"));
        Y_CALL(testNextToken("/", "", TokenType::INVALID_TOKEN, "/"));
        Y_CALL(testNextToken("//", TokenType::END_OF_BUFFER, "//"));
        Y_CALL(testNextToken("//", "", TokenType::COMMENT, "//"));
        Y_CALL(testNextToken("//\n", TokenType::COMMENT, "//"));
        Y_CALL(testNextToken("//\r", TokenType::COMMENT, "//"));
        Y_CALL(testNextToken("// 12 { } [ ] \"\"\n", TokenType::COMMENT,
                             "// 12 { } [ ] \"\""));

        Y_CALL(testNextToken("*/ ", TokenType::VALUE, "*/"));
        Y_CALL(testNextToken("*/", "", TokenType::VALUE, "*/"));
        Y_CALL(testNextToken("/* \n*/", TokenType::BLOCK_COMMENT,
                             "/* \n*/"));
        Y_CALL(testNextToken("/* ", " * / */", TokenType::BLOCK_COMMENT,
                             "/*  * / */"));
        Y_CALL(testNextToken("/* ", " /* // */", TokenType::BLOCK_COMMENT,
                             "/*  /* // */"));
        Y_CALL(testNextToken("/* ", " /* // */*/", TokenType::BLOCK_COMMENT,
                             "/*  /* // */"));
        Y_CALL(testNextToken("/* ", "", TokenType::INVALID_TOKEN,
                             "/* "));
    }

    void test_WhitespaceTokens()
    {
        Y_CALL(testNextToken(" 1", TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken("\t1", TokenType::WHITESPACE, "\t"));
        Y_CALL(testNextToken("\t 1", TokenType::WHITESPACE, "\t "));
        Y_CALL(testNextToken(" \t 1", TokenType::WHITESPACE, " \t "));
        Y_CALL(testNextToken(" ", "", TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken("\t", "", TokenType::WHITESPACE, "\t"));
        Y_CALL(testNextToken("\t", " 1", TokenType::WHITESPACE, "\t "));
        Y_CALL(testNextToken(" \t", " ", "", TokenType::WHITESPACE, " \t "));
    }

    void testStructureToken(const std::string token, TokenType expectedType)
    {
        Y_CALL(testNextToken(token, expectedType, token));
        Y_CALL(testNextToken(token + token, expectedType, token));
        Y_CALL(testNextToken(token + " ", expectedType, token));
        Y_CALL(testNextToken(token + "\"", expectedType, token));
    }

    void test_StructureTokens()
    {
        Y_CALL(testStructureToken("{", TokenType::START_OBJECT));
        Y_CALL(testStructureToken("}", TokenType::END_OBJECT));
        Y_CALL(testStructureToken("[", TokenType::START_ARRAY));
        Y_CALL(testStructureToken("]", TokenType::END_ARRAY));
        Y_CALL(testStructureToken(":", TokenType::COLON));
        Y_CALL(testStructureToken(",", TokenType::COMMA));
    }

    std::string json1 = "\
{\n\
    \"text\": \"A value with\\n\\\"escaped characters\\\"\",\n\
    \"list\": [12, 12.10],\r\n\
    \"true bool\": true,\n\
    \"false bool\": false,\n\
    \"null\": null\n\
}";

    void testNextToken(Tokenizer& tokenizer, TokenType type,
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
        Tokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), json1.length());
        Y_CALL(testNextToken(tokenizer, TokenType::START_OBJECT, "{"));
        Y_CALL(testNextToken(tokenizer, TokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, TokenType::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, TokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, TokenType::STRING,
                             "\"A value with\\n\\\"escaped characters\\\"\""));
        Y_CALL(testNextToken(tokenizer, TokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, TokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, TokenType::STRING, "\"list\""));
        Y_CALL(testNextToken(tokenizer, TokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, TokenType::START_ARRAY, "["));
        Y_CALL(testNextToken(tokenizer, TokenType::VALUE, "12"));
        Y_CALL(testNextToken(tokenizer, TokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, TokenType::VALUE, "12.10"));
        Y_CALL(testNextToken(tokenizer, TokenType::END_ARRAY, "]"));
        Y_CALL(testNextToken(tokenizer, TokenType::COMMA, ","));
        Y_CALL(testNextToken(tokenizer, TokenType::NEWLINE, "\r\n"));
    }

    void test_MultiBuffer()
    {
        size_t splits[] = {28};
        Tokenizer tokenizer;
        tokenizer.setBuffer(json1.c_str(), splits[0]);
        Y_CALL(testNextToken(tokenizer, TokenType::START_OBJECT, "{"));
        Y_CALL(testNextToken(tokenizer, TokenType::NEWLINE, "\n"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, "    "));
        Y_CALL(testNextToken(tokenizer, TokenType::STRING, "\"text\""));
        Y_CALL(testNextToken(tokenizer, TokenType::COLON, ":"));
        Y_CALL(testNextToken(tokenizer, TokenType::WHITESPACE, " "));
        Y_CALL(testNextToken(tokenizer, TokenType::END_OF_BUFFER,
                             "\"A value with\\"));
        tokenizer.setBuffer(json1.c_str() + splits[0],
                            json1.size() - splits[0]);
        Y_CALL(testNextToken(tokenizer, TokenType::STRING,
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
