//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonInvalidValue.hpp"

namespace Yson
{
    namespace
    {
        const std::string InvalidTypeName = "Invalid";
    }

    const JsonInvalidValue& JsonInvalidValue::instance()
    {
        static JsonInvalidValue value;
        return value;
    }

    ValueType JsonInvalidValue::valueType() const
    {
        return ValueType::INVALID;
    }

    const std::string& JsonInvalidValue::typeName() const
    {
        return InvalidTypeName;
    }

    bool JsonInvalidValue::value(std::string& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(bool& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(int8_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(int16_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(int32_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(int64_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(uint8_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(uint16_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(uint32_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(uint64_t& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(float& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(double& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(long double& value) const
    {
        return false;
    }

    bool JsonInvalidValue::value(std::vector<char>& value) const
    {
        return false;
    }

    const JsonValue& JsonInvalidValue::operator[](size_t index) const
    {
        return *this;
    }

    const JsonValue& JsonInvalidValue::operator[](const std::string& key) const
    {
        return *this;
    }

    size_t JsonInvalidValue::updateStringRef(size_t offset)
    {
        return offset;
    }
}
