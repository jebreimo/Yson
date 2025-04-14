//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader.hpp"
#include "Yson/UBJsonReader.hpp"

#include <sstream>
#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_readItem_basics()
    {
        std::string doc = R"({"foo": "bar", "zap": [1, 2, 3], "folderNix": "/path/to/folder", "folderWin": "c:\\path\\to\\folder"})";
        JsonReader reader(doc.data(), doc.size());
        auto item = reader.readItem();
        Y_ASSERT(item.isObject());
        Y_ASSERT(get<std::string>(item["foo"]) == "bar");
        Y_ASSERT(get<std::string>(item["folderNix"]) == "/path/to/folder");
        Y_ASSERT(get<std::string>(item["folderWin"]) == "c:\\path\\to\\folder");
        Y_ASSERT(get<int32_t>(item["zap"][2]) == 3);

        Y_THROWS(item["bob"], YsonException);
        Y_THROWS(item[0], YsonException);

        Y_ASSERT(item.get("foo") != nullptr);
        Y_ASSERT(item.get("bob") == nullptr);
        Y_THROWS(auto p = item.get(0), YsonException);

        Y_EQUAL(get<std::string>(item.get("foo"), "cup"), "bar");
        Y_EQUAL(get<std::string>(item.get("bob"), "cup"), "cup");
    }

    void test_integerItem()
    {
        std::string doc = R"(1234)";
        JsonReader reader(doc.data(), doc.size());
        auto item = reader.readItem();
        Y_ASSERT(item.isValue());
        Y_ASSERT(get<std::string>(item) == "1234");
        Y_ASSERT(get<int32_t>(item) == 1234);
    }

    void test_ub_readItem_basics()
    {
        std::string doc("{i\x03KeySi\x0CHello world!"
                        "i\x05" "Array[I\x10\x20U\xF0]}");
        UBJsonReader reader(doc.data(), doc.size());
        auto item = reader.readItem();
        Y_ASSERT(item.isObject());
        Y_ASSERT(get<std::string>(item["Key"]) == "Hello world!");
        Y_ASSERT(get<int>(item["Array"][1]) == 240);
    }

    void test_ub_binary_item()
    {
        const char doc[] = "[$i#i\4AB D";

        UBJsonReader reader1(doc, sizeof(doc) - 1);
        auto item1 = reader1.readItem();
        Y_ASSERT(item1.isArray());
        Y_ASSERT(item1.array().values().size() == 4);
        Y_ASSERT(get<int>(item1[3]) == 'D');

        UBJsonReader reader2(doc, sizeof(doc) - 1);
        reader2.setExpandOptimizedByteArraysEnabled(false);
        auto item2 = reader2.readItem();
        Y_ASSERT(item2.isValue());
        size_t size = 0;
        Y_ASSERT(item2.value().getBinary(nullptr, size));
        Y_ASSERT(size == 4);
        std::string buffer(size, {});
        Y_ASSERT(item2.value().getBinary(buffer.data(), size));
        Y_ASSERT(buffer == "AB D");
    }

    Y_TEST(test_readItem_basics,
           test_integerItem,
           test_ub_readItem_basics,
           test_ub_binary_item);
}
