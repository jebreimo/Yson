//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader.hpp"
#include "Yson/UBJsonReader.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void assertIsJson(const std::string& s)
    {
        Y_ASSERT(dynamic_cast<JsonReader*>(makeReader(s.data(), s.size()).get()));
    }

    void assertIsUBJson(const std::string& s)
    {
        Y_ASSERT(dynamic_cast<UBJsonReader*>(makeReader(s.data(), s.size()).get()));
    }

    void assertFails(const std::string& s)
    {
        Y_THROWS(makeReader(s.data(), s.size()), YsonException);
    }

    void test_MakeReader()
    {
        Y_CALL(assertIsJson(""));
        Y_CALL(assertIsJson("1"));
        Y_CALL(assertIsJson("[1]"));
        Y_CALL(assertIsJson("[{},{}]"));
        Y_CALL(assertIsJson("{\n}"));
        Y_CALL(assertIsJson("{// Start\n}"));
        Y_CALL(assertIsJson("null"));
        Y_CALL(assertIsJson("{\"1\":2}"));
        Y_CALL(assertIsUBJson("[#\x01iA"));
        Y_CALL(assertIsUBJson("IAA"));
        Y_CALL(assertFails("Pluto"));
    }

    void test_MakeReaderWithStream()
    {
        std::istringstream ss(R"({"a": 1})");
        auto reader = makeReader(ss);
        Y_ASSERT(reader->nextValue());
    }

    Y_TEST(test_MakeReader,
           test_MakeReaderWithStream);
}
