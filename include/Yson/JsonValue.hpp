//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "ValueType.hpp"
#include "YsonException.hpp"

namespace Yson
{
    class JsonValue
    {
    public:
        JsonValue(std::string value, int tokenType);

        [[nodiscard]]
        ValueType valueType(bool analyzeStrings = false) const;

        [[nodiscard]]
        bool isNull() const;

        bool get(bool& value) const;

        bool get(int8_t& value) const;

        bool get(int16_t& value) const;

        bool get(int32_t& value) const;

        bool get(int64_t& value) const;

        bool get(uint8_t& value) const;

        bool get(uint16_t& value) const;

        bool get(uint32_t& value) const;

        bool get(uint64_t& value) const;

        bool get(float& value) const;

        bool get(double& value) const;

        bool get(long double& value) const;

        bool get(char& value) const;

        bool get(std::string& value) const;

        bool getBase64(std::vector<char>& value) const;

        bool getBinary(std::vector<char>& value) const;

        bool getBinary(void* buffer, size_t& size) const;

    private:
        std::string m_Value;
        int m_Type;
    };

    template <typename T>
    T get(const JsonValue& value)
    {
        T v;
        if (!value.get(v))
        {
            throw YsonException("get called with incorrect type."
                                " The value type is"
                                + toString(value.valueType()) + ".",
                                __FILE__, __LINE__, __FUNCTION__);
        }
        return v;
    }
}
