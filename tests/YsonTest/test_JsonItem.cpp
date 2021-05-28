//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader.hpp"

#include <sstream>
#include "Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    void test_readItem_basics()
    {
        std::string doc = R"({"foo": "bar", "zap": [1, 2, 3]})";
        JsonReader reader(doc.data(), doc.size());
        auto item = reader.readCurrentItem();
        Y_ASSERT(item.isObject());
        Y_ASSERT(get<std::string>(item["foo"]) == "bar");
        Y_ASSERT(get<int32_t>(item["zap"][2]) == 3);
    }

    void test_integerItem()
    {
        std::string doc = R"(1234)";
        JsonReader reader(doc.data(), doc.size());
        auto item = reader.readCurrentItem();
        Y_ASSERT(item.isValue());
        Y_ASSERT(get<std::string>(item) == "1234");
        Y_ASSERT(get<int32_t>(item) == 1234);
    }

    Y_TEST(test_readItem_basics,
           test_integerItem);
}
