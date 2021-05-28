//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonItem.hpp"
#include "Yson/JsonArray.hpp"
#include "Yson/JsonObject.hpp"
#include "Yson/Common/ThrowYsonException.hpp"

namespace Yson
{
    JsonItem::JsonItem(JsonItem::InternalItem item)
        : m_Item(move(item))
    {}

    const JsonItem& JsonItem::operator[](size_t index) const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<JsonArray>>(&m_Item))
        {
            const auto& values = (*obj)->values();
            if (index < values.size())
                return values[index];
            YSON_THROW("Index is too great: " + std::to_string(index));
        }
        YSON_THROW("Item isn't an array.");
    }

    const JsonItem& JsonItem::operator[](std::string_view key) const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<JsonObject>>(&m_Item))
        {
            const auto& values = (*obj)->values();
            auto it = values.find(key);
            if (it != values.end())
                return it->second;
            YSON_THROW("No such key: " + std::string(key));
        }
        YSON_THROW("Item isn't an object.");
    }

    bool JsonItem::isArray() const
    {
        return std::holds_alternative<std::shared_ptr<JsonArray>>(m_Item);
    }

    const JsonArray& JsonItem::array() const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<JsonArray>>(&m_Item))
            return **obj;
        YSON_THROW("Item isn't an array.");
    }

    bool JsonItem::isObject() const
    {
        return std::holds_alternative<std::shared_ptr<JsonObject>>(m_Item);
    }

    const JsonObject& JsonItem::object() const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<JsonObject>>(&m_Item))
            return **obj;
        YSON_THROW("Item isn't an object.");
    }

    bool JsonItem::isValue() const
    {
        return std::holds_alternative<JsonValue>(m_Item);
    }

    const JsonValue& JsonItem::value() const
    {
        if (const auto* obj = std::get_if<JsonValue>(&m_Item))
            return *obj;
        YSON_THROW("Item isn't a value.");
    }
}
