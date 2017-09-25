//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 07.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/UBJsonReader/UBJsonTokenizer.hpp"
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_Basics()
    {
        char buffer[] = "{I\x10\x20}";
        UBJsonTokenizer tokenizer(buffer, sizeof(buffer) - 1);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), UBJsonTokenType::START_OBJECT_TOKEN);
        Y_EQUAL(tokenizer.tokenSize(), 1);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), UBJsonTokenType::INT16_TOKEN);
        Y_EQUAL(tokenizer.tokenSize(), 2);
        Y_EQUAL(tokenizer.tokenAs<int16_t>(), 0x1020);
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), UBJsonTokenType::END_OBJECT_TOKEN);
        Y_EQUAL(tokenizer.tokenSize(), 1);
        Y_ASSERT(!tokenizer.next());
    }

    template <typename T>
    void testToken(const std::string& buffer,
                   UBJsonTokenType expectedTokenType,
                   T expectedValue)
    {
        UBJsonTokenizer tokenizer(buffer.data(), buffer.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), expectedTokenType);
        Y_EQUAL(tokenizer.tokenSize(), sizeof(T));
        Y_EQUAL(tokenizer.tokenAs<T>(), expectedValue);
    }

    void testStringToken(const std::string& buffer,
                         UBJsonTokenType expectedTokenType,
                         const std::string& expectedValue)
    {
        UBJsonTokenizer tokenizer(buffer.data(), buffer.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), expectedTokenType);
        Y_EQUAL(tokenizer.tokenSize(), expectedValue.size());
        auto token = tokenizer.token();
        std::string value(token.data(), token.size());
        Y_EQUAL(value, expectedValue);
    }

    void test_Token()
    {
        Y_CALL(testToken("i\x10", UBJsonTokenType::INT8_TOKEN, int8_t(0x10)));
        Y_CALL(testToken("L\x10\x20\x30\x40\x50\x60\x70\x80",
                         UBJsonTokenType::INT64_TOKEN,
                         int64_t(0x1020304050607080)));
        Y_CALL(testStringToken("Si\x0CHello world!",
                               UBJsonTokenType::STRING_TOKEN,
                               "Hello world!"));
    }

    void test_OptimizedArray()
    {
        std::string doc = "[$d#U\xCA";
        UBJsonTokenizer tokenizer(doc.data(), doc.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(),
                UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN);
        Y_EQUAL(tokenizer.contentSize(), 0xCA);
        Y_EQUAL(tokenizer.contentType(), UBJsonTokenType::FLOAT32_TOKEN);
    }

    void test_PartiallyOptimizedArray()
    {
        std::string doc = "[#U\xCA";
        UBJsonTokenizer tokenizer(doc.data(), doc.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(),
                UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN);
        Y_EQUAL(tokenizer.contentSize(), 0xCA);
        Y_EQUAL(tokenizer.contentType(), UBJsonTokenType::UNKNOWN_TOKEN);
    }

    void test_OptimizedObject()
    {
        std::string doc = "{$d#U\xCA";
        UBJsonTokenizer tokenizer(doc.data(), doc.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(),
                UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN);
        Y_EQUAL(tokenizer.contentSize(), 0xCA);
        Y_EQUAL(tokenizer.contentType(), UBJsonTokenType::FLOAT32_TOKEN);
    }

    void test_PartiallyOptimizedObject()
    {
        std::string doc = "{#U\xCA";
        UBJsonTokenizer tokenizer(doc.data(), doc.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(),
                UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN);
        Y_EQUAL(tokenizer.contentSize(), 0xCA);
        Y_EQUAL(tokenizer.contentType(), UBJsonTokenType::UNKNOWN_TOKEN);
    }

    void test_IgnoreNoOp()
    {
        std::string doc = "NZ";
        UBJsonTokenizer tokenizer(doc.data(), doc.size());
        Y_ASSERT(tokenizer.next());
        Y_EQUAL(tokenizer.tokenType(), UBJsonTokenType::NULL_TOKEN);
    }

    Y_TEST(test_Basics,
           test_Token,
           test_OptimizedArray,
           test_OptimizedObject,
           test_PartiallyOptimizedArray,
           test_PartiallyOptimizedObject,
           test_IgnoreNoOp);
}
