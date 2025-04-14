//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader/JsonTokenizer.hpp"
#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        char text[] = R"({"key": 123, "key2": "value"})";
        JsonTokenizer tokenizer(text, sizeof(text));
        tokenizer.setChunkSize(4);

        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::START_OBJECT);
        Y_EQUAL(tokenizer.token(), "{");
        Y_EQUAL(tokenizer.lineNumber(), 1);
        Y_EQUAL(tokenizer.columnNumber(), 2);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::STRING);
        Y_EQUAL(tokenizer.token(), "key");
        Y_EQUAL(tokenizer.lineNumber(), 1);
        Y_EQUAL(tokenizer.columnNumber(), 7);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::COLON);
        Y_EQUAL(tokenizer.token(), ":");
        Y_EQUAL(tokenizer.columnNumber(), 8);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::VALUE);
        Y_EQUAL(tokenizer.token(), "123");
        Y_EQUAL(tokenizer.columnNumber(), 12);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::COMMA);
        Y_EQUAL(tokenizer.token(), ",");
        Y_EQUAL(tokenizer.columnNumber(), 13);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::STRING);
        Y_EQUAL(tokenizer.token(), "key2");
        Y_EQUAL(tokenizer.columnNumber(), 20);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::COLON);
        Y_EQUAL(tokenizer.token(), ":");
        Y_EQUAL(tokenizer.columnNumber(), 21);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::STRING);
        Y_EQUAL(tokenizer.token(), "value");
        Y_EQUAL(tokenizer.columnNumber(), 29);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::END_OBJECT);
        Y_EQUAL(tokenizer.token(), "}");
        Y_EQUAL(tokenizer.columnNumber(), 30);
        Y_ASSERT(!tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::END_OF_FILE);
        Y_EQUAL(tokenizer.columnNumber(), 30);
        Y_ASSERT(!tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::END_OF_FILE);
        Y_EQUAL(tokenizer.columnNumber(), 30);
    }

    void test_Value()
    {
        char text[] = R"(null)";
        JsonTokenizer tokenizer(text, sizeof(text));
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), JsonTokenType::VALUE);
        Y_EQUAL(tokenizer.token(), "null");
    }

    void testNextToken(const std::string& input,
                       JsonTokenType expectedType,
                       const std::string& expectedToken)
    {
        JsonTokenizer tokenizer(input.data(), input.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), expectedType);
        Y_EQUAL(tokenizer.token(), expectedToken);
    }

    void testNoNextToken(const std::string& input, JsonTokenType expectedType,
                         const std::string& expectedToken)
    {
        JsonTokenizer tokenizer(input.data(), input.size());
        Y_ASSERT(!tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), expectedType);
        Y_EQUAL(tokenizer.token(), expectedToken);
    }

    void test_StringTokens()
    {
        Y_CALL(testNoNextToken("\"", JsonTokenType::INVALID_TOKEN, "\""));
        Y_CALL(testNextToken("\"\" ", JsonTokenType::STRING, ""));
        Y_CALL(testNoNextToken("\"\n", JsonTokenType::INVALID_TOKEN, "\""));
        Y_CALL(testNextToken("\"\"\"", JsonTokenType::STRING, ""));
        Y_CALL(testNextToken("\"\\\"\"", JsonTokenType::STRING, "\\\""));
        Y_CALL(testNextToken("\"\\\\\"", JsonTokenType::STRING, "\\\\"));
        Y_CALL(testNextToken("\"\\\\\\\"\"", JsonTokenType::STRING,
                             "\\\\\\\""));
    }

    void test_SingleQuotedStringTokens()
    {
        Y_CALL(testNextToken("''", JsonTokenType::STRING, ""));
        Y_CALL(testNextToken("'foo \\'bar\\' baz'", JsonTokenType::STRING,
                             "foo \\'bar\\' baz"));
    }

    void test_Newlines()
    {
        Y_CALL(testNextToken("\r\"string\"", JsonTokenType::STRING, "string"));
        Y_CALL(testNextToken("\r\n2", JsonTokenType::VALUE, "2"));
        Y_CALL(testNextToken("\n2", JsonTokenType::VALUE, "2"));
        Y_CALL(testNextToken("\n\n\n2", JsonTokenType::VALUE, "2"));
    }

    void test_Comments()
    {
        Y_CALL(testNoNextToken("/", JsonTokenType::INVALID_TOKEN, "/"));
        Y_CALL(testNoNextToken("//", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNoNextToken("//\n", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNoNextToken("//\r", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNextToken("// 12 { } [ ] \"\"\n1234", JsonTokenType::VALUE,
                             "1234"));

        Y_CALL(testNextToken("*/ ", JsonTokenType::VALUE, "*/"));
        Y_CALL(testNoNextToken("/* \n*/", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNoNextToken("/*  * / */", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNextToken("/* /* // */45", JsonTokenType::VALUE,
                             "45"));
        Y_CALL(testNoNextToken("/* ", JsonTokenType::INVALID_TOKEN, "/* "));
        Y_CALL(testNextToken("23/* ", JsonTokenType::VALUE, "23"));
    }

    void test_ValueTokens()
    {
        Y_CALL(testNextToken("8", JsonTokenType::VALUE, "8"));
        Y_CALL(testNextToken("---", JsonTokenType::VALUE, "---"));
        Y_CALL(testNextToken("---", JsonTokenType::VALUE, "---"));
        Y_CALL(testNextToken("--\" ", JsonTokenType::VALUE, "--"));
        Y_CALL(testNoNextToken("/a ", JsonTokenType::INVALID_TOKEN, "/a"));
        Y_CALL(testNextToken("8:", JsonTokenType::VALUE, "8"));
    }

    void test_Whitespaces()
    {
        Y_CALL(testNextToken(" 1", JsonTokenType::VALUE, "1"));
        Y_CALL(testNextToken("\t1", JsonTokenType::VALUE, "1"));
        Y_CALL(testNextToken("\t 1", JsonTokenType::VALUE, "1"));
        Y_CALL(testNextToken(" \t 1", JsonTokenType::VALUE, "1"));
        Y_CALL(testNoNextToken("  \n  ", JsonTokenType::END_OF_FILE, ""));
        Y_CALL(testNoNextToken("\t", JsonTokenType::END_OF_FILE, ""));
    }

    void test_MultilineStrings()
    {
        Y_CALL(testNextToken("'ABC \\\nDEF'", JsonTokenType::STRING,
                             "ABC DEF"));
        Y_CALL(testNextToken("'ABC \\\rDEF'", JsonTokenType::STRING,
                             "ABC DEF"));
        Y_CALL(testNextToken("'ABC \\\r\nDEF'", JsonTokenType::STRING,
                             "ABC DEF"));
        Y_CALL(testNoNextToken("'ABC \\\n\rDEF'",
                               JsonTokenType::INVALID_TOKEN,
                               "'ABC \\\n\rDEF'"));
    }

    void test_StreamAndBuffer()
    {
        std::stringstream ss;
        std::string buffer("{}");
        JsonTokenizer tokenizer(ss, buffer.data(), buffer.size());
        Y_ASSERT(tokenizer.next());
    }

    void test_IncompleteUtf8()
    {
        std::stringstream ss;
        ss.write(R"({"Key": )", 7);
        JsonTokenizer tokenizer(ss);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.token(), "{");
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.token(), "Key");
        ss.clear();
        ss.write("1, \"", 4);
        ss.put('\xC0');
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.token(), ":");
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.token(), "1");
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.token(), ",");
        Y_ASSERT(!tokenizer.next());
    }

    Y_TEST(test_Basics,
           test_StringTokens,
           test_SingleQuotedStringTokens,
           test_Newlines,
           test_Comments,
           test_Value,
           test_ValueTokens,
           test_Whitespaces,
           test_MultilineStrings,
           test_StreamAndBuffer,
           test_IncompleteUtf8);
}
