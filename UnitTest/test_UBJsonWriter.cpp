//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../Yson/UBJsonWriter.hpp"
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    template <int N>
    std::string S(const char (&s)[N])
    {
        return std::string(s, s + N - 1);
    }

    void test_Integer()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        writer.value(int64_t(0x00FF00FF00FF00FFLL));
        writer.flush();
        Y_EQUAL(stream.str(), S("L\x00\xFF\x00\xFF\x00\xFF\x00\xFF"));
    }

    void test_Array()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        writer.key("Key");
        Y_EQUAL(writer.key(), "Key");
        writer.beginArray().value(int32_t(0xFFBB7733))
                .endArray().flush();
        Y_EQUAL(stream.str(), S("[l\xFF\xBB\x77\x33]"));
    }

    void test_Object()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        writer.beginObject().key("Key").value(int32_t(0xFFBB7733))
                .endObject().flush();
        Y_EQUAL(stream.str(), S("{U\x03Keyl\xFF\xBB\x77\x33}"));
    }

    void test_OptimizedArray()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        writer.beginArray(UBJsonParameters(3, UBJsonValueType::INT_16)).value(
                int16_t(2)).value(int16_t(200)).value(int16_t(20000))
                .endArray().flush();
        Y_EQUAL(stream.str(), S("[$I#i\x03" "\x00\x02\x00\xC8\x4E\x20"));
    }

    void test_WriteBinary()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        int8_t data[] = {1, 2, 3, 5, 8, 14};
        writer.binary(data, sizeof(data)).flush();
        Y_EQUAL(stream.str(), S("[$U#i\x06" "\x01\x02\x03\x05\x08\x0E"));
    }

    void test_WriteString()
    {
        std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
        UBJsonWriter writer(stream);
        writer.value("Snorre").flush();
        Y_EQUAL(stream.str(), S("SU\x06Snorre"));
    }

    Y_TEST(test_Integer,
           test_Array,
           test_Object,
           test_OptimizedArray,
           test_WriteBinary,
           test_WriteString);
}
