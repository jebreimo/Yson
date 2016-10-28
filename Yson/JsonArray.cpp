//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonArray.hpp"

#include "JsonInvalidValue.hpp"

namespace Yson
{
    namespace
    {
        const std::string ArrayTypeName = "Array";
    }

    JsonArray::JsonArray(std::vector<std::unique_ptr<JsonValue>>&& values)
        : m_Values(move(values))
    {}

    ValueType JsonArray::valueType() const
    {
        return ValueType::ARRAY;
    }

    const std::string& JsonArray::typeName() const
    {
        return ArrayTypeName;
    }

    bool JsonArray::empty() const
    {
        return m_Values.empty();
    }

    size_t JsonArray::size() const
    {
        return m_Values.size();
    }

    const JsonValue& JsonArray::operator[](size_t index) const
    {
        if (index < m_Values.size())
            return *m_Values[index];
        return JsonInvalidValue::instance();
    }

    std::vector<std::unique_ptr<JsonValue>>::const_iterator
    JsonArray::begin() const
    {
        return m_Values.begin();
    }

    std::vector<std::unique_ptr<JsonValue>>::const_iterator
    JsonArray::end() const
    {
        return m_Values.end();
    }

    size_t JsonArray::updateStringRef(size_t offset)
    {
        for (auto& value : m_Values)
            offset = value->updateStringRef(offset);
        return offset;
    }
}
