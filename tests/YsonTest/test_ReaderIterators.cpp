//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-06-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/ReaderIterators.hpp"

#include "Ytest/Ytest.hpp"

namespace
{
    void test_ObjectKeyIterator()
    {
        std::string doc = R"-({"foo": "bar", "val": 123, "zap": {"a": false, "b": 1.0}})-";
        auto reader = Yson::makeReader(doc.data(), doc.size());
        std::vector<std::string> keys1 = {"foo", "val", "zap"};
        int i = 0;
        for (auto& key : keys(*reader))
        {
            Y_EQUAL(key, keys1.at(i++));
            if (key == "foo")
            {
                Y_EQUAL(Yson::read<std::string>(*reader), "bar");
            }
            else if (key == "val")
            {
                Y_EQUAL(Yson::read<int32_t>(*reader), 123);
            }
            else if (key == "zap")
            {
                std::vector<std::string> keys2 = {"a", "b"};
                int j = 0;
                for (auto& key2 : keys(*reader))
                {
                    Y_EQUAL(key2, keys2.at(j++));
                }
                Y_EQUAL(j, 2);
            }
        }
        Y_EQUAL(i, 3);
    }

    void test_ArrayIterator()
    {
        std::string doc = R"-(["ab", 12, [1.1, true]])-";
        auto reader = Yson::makeReader(doc.data(), doc.size());
        int i = 0;
        for (Yson::ArrayIterator it(*reader); it.next();)
        {
            if (i == 0)
            {
                Y_EQUAL(Yson::read<std::string>(*reader), "ab");
            }
            else if (i == 1)
            {
                Y_EQUAL(Yson::read<int>(*reader), 12);
            }
            else if (i == 2)
            {
                int j = 0;
                for (Yson::ArrayIterator it2(*reader); it2.next();)
                {
                    if (j == 0)
                        Y_EQUAL(Yson::read<double>(*reader), 1.1);
                    else if (j == 1)
                        Y_EQUAL(Yson::read<bool>(*reader), true);
                    ++j;
                }
                Y_EQUAL(j, 2);
            }
            ++i;
        }
        Y_EQUAL(i, 3);
    }

    void test_ArrayValueIterator()
    {
        std::string doc = R"-([1, 2, 3, 4, 5, 6])-";
        auto reader = Yson::makeReader(doc.data(), doc.size());
        auto adapter = Yson::arrayValues<int>(*reader);
        std::vector<int> values(adapter.begin(), adapter.end());
        Y_EQUAL(values.size(), 6);
        Y_EQUAL(values[0], 1);
        Y_EQUAL(values[5], 6);
    }

    Y_TEST(test_ObjectKeyIterator,
           test_ArrayIterator,
           test_ArrayValueIterator);
}
