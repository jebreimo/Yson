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

        ValueType valueType() const override;

        const std::string& typeName() const override;

        bool value(std::string& value) const override;

        bool value(bool& value) const override;

        bool value(int8_t& value) const override;

        bool value(int16_t& value) const override;

        bool value(int32_t& value) const override;

        bool value(int64_t& value) const override;

        bool value(uint8_t& value) const override;

        bool value(uint16_t& value) const override;

        bool value(uint32_t& value) const override;

        bool value(uint64_t& value) const override;

        bool value(float& value) const override;

        bool value(double& value) const override;

        bool value(long double& value) const override;

        bool value(std::vector<uint8_t>& value) const override;

        const JsonValue& operator[](size_t index) const override;

        const JsonValue& operator[](const std::string& key) const override;

        size_t updateStringRef(size_t offset) override;
    private:
        JsonInvalidValue() = default;

        JsonInvalidValue(const JsonInvalidValue&) = delete;

        JsonInvalidValue(JsonInvalidValue&&) = delete;
    };
}


