//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "ValueType.hpp"
#include "YsonException.hpp"

namespace Yson
{
    class YSON_API ValueItem
    {
    public:
        virtual ~ValueItem() = default;

        [[nodiscard]]
        virtual ValueType valueType() const = 0;

        [[nodiscard]]
        virtual ValueType valueType(bool analyzeStrings) const = 0;

        [[nodiscard]]
        virtual bool isNull() const = 0;

        virtual bool get(bool& value) const = 0;

        virtual bool get(int8_t& value) const = 0;

        virtual bool get(int16_t& value) const = 0;

        virtual bool get(int32_t& value) const = 0;

        virtual bool get(int64_t& value) const = 0;

        virtual bool get(uint8_t& value) const = 0;

        virtual bool get(uint16_t& value) const = 0;

        virtual bool get(uint32_t& value) const = 0;

        virtual bool get(uint64_t& value) const = 0;

        virtual bool get(float& value) const = 0;

        virtual bool get(double& value) const = 0;

        virtual bool get(long double& value) const = 0;

        virtual bool get(char& value) const = 0;

        virtual bool get(std::string& value) const = 0;

        virtual bool getBase64(std::vector<char>& value) const = 0;

        virtual bool getBinary(std::vector<char>& value) const = 0;

        virtual bool getBinary(void* buffer, size_t& size) const = 0;
    };

    template <typename T>
    T get(const ValueItem& value)
    {
        T v;
        if (!value.get(v))
        {
            YSON_THROW("get(...) called with incorrect type."
                       " The actual value type is "
                       + toString(value.valueType()) + ".");
        }
        return v;
    }
}
