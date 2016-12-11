//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonValue.hpp"

#include "YsonException.hpp"

namespace Yson
{
    JsonValue::JsonValue() = default;

    bool JsonValue::value(std::string& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(bool& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(int8_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(int16_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(int32_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(int64_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(uint8_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(uint16_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(uint32_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(uint64_t& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(float& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(double& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(long double& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    bool JsonValue::value(std::vector<char>& value) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    const JsonValue& JsonValue::operator[](size_t index) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }

    const JsonValue& JsonValue::operator[](const std::string& key) const
    {
        YSON_THROW("Unsupported function for values of type " + typeName());
    }
}
