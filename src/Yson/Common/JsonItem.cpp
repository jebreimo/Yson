//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonItem.hpp"
#include "Yson/ArrayItem.hpp"
#include "Yson/ObjectItem.hpp"
#include "Yson/YsonException.hpp"

namespace Yson
{
    JsonItem::JsonItem(JsonItem::ItemType item)
        : m_Item(move(item))
    {}

    const JsonItem& JsonItem::operator[](size_t index) const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<ArrayItem>>(&m_Item))
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
        if (const auto* obj = std::get_if<std::shared_ptr<ObjectItem>>(&m_Item))
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
        return std::holds_alternative<std::shared_ptr<ArrayItem>>(m_Item);
    }

    const ArrayItem& JsonItem::array() const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<ArrayItem>>(&m_Item))
            return **obj;
        YSON_THROW("Item isn't an array.");
    }

    bool JsonItem::isObject() const
    {
        return std::holds_alternative<std::shared_ptr<ObjectItem>>(m_Item);
    }

    const ObjectItem& JsonItem::object() const
    {
        if (const auto* obj = std::get_if<std::shared_ptr<ObjectItem>>(&m_Item))
            return **obj;
        YSON_THROW("Item isn't an object.");
    }

    bool JsonItem::isValue() const
    {
        return std::holds_alternative<JsonValueItem>(m_Item)
               || std::holds_alternative<UBJsonValueItem>(m_Item);
    }

    const ValueItem& JsonItem::value() const
    {
        if (const auto* obj = std::get_if<JsonValueItem>(&m_Item))
            return *obj;
        if (const auto* obj = std::get_if<UBJsonValueItem>(&m_Item))
            return *obj;
        YSON_THROW("Item isn't a value.");
    }
}
