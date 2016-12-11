//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonValue.hpp"

namespace Yson
{
    class JsonInvalidValue : public JsonValue
    {
    public:
        static const JsonInvalidValue& instance();


        ValueType valueType() const final;

        const std::string& typeName() const final;

        bool value(std::string& value) const final;

        bool value(bool& value) const final;

        bool value(int8_t& value) const final;

        bool value(int16_t& value) const final;

        bool value(int32_t& value) const final;

        bool value(int64_t& value) const final;

        bool value(uint8_t& value) const final;

        bool value(uint16_t& value) const final;

        bool value(uint32_t& value) const final;

        bool value(uint64_t& value) const final;

        bool value(float& value) const final;

        bool value(double& value) const final;

        bool value(long double& value) const final;

        bool value(std::vector<char>& value) const final;

        const JsonValue& operator[](size_t index) const final;

        const JsonValue& operator[](const std::string& key) const final;

        size_t updateStringRef(size_t offset) final;
    private:
        JsonInvalidValue() = default;

        JsonInvalidValue(const JsonInvalidValue&) = delete;

        JsonInvalidValue(JsonInvalidValue&&) = delete;
    };
}


