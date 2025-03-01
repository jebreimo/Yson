//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <variant>
#include "JsonValueItem.hpp"
#include "UBJsonValueItem.hpp"

namespace Yson
{
    class ArrayItem;
    class ObjectItem;

    /**
     * @brief A class that can hold any type of JSON item (object, array,
     *  value).
     *
     * Instances of JsonItem are created by Reader::readItem() in addition
     * to JsonItem itself.
     */
    class YSON_API JsonItem
    {
    public:
        using ItemType = std::variant<std::shared_ptr<ObjectItem>,
                                      std::shared_ptr<ArrayItem>,
                                      JsonValueItem,
                                      UBJsonValueItem>;

        explicit JsonItem(ItemType item);

        const JsonItem& operator[](std::string_view key) const;

        const JsonItem& operator[](size_t index) const;

        [[nodiscard]]
        const JsonItem* get(std::string_view key) const;

        [[nodiscard]]
        const JsonItem* get(size_t index) const;

        [[nodiscard]]
        bool isArray() const;

        [[nodiscard]]
        const ArrayItem& array() const;

        [[nodiscard]]
        bool isObject() const;

        [[nodiscard]]
        const ObjectItem& object() const;

        [[nodiscard]]
        bool isValue() const;

        [[nodiscard]]
        const ValueItem& value() const;
    private:
        ItemType m_Item;
    };

    template <typename T>
    T get(const JsonItem& item)
    {
        return get<T>(item.value());
    }

    template <typename T>
    T get(const JsonItem* item, const T& defaultValue)
    {
        return item ? get<T>(item->value()) : defaultValue;
    }

    inline std::string get(const JsonItem* item, const std::string_view& defaultValue)
    {
        return item ? get<std::string>(item->value()) : std::string(defaultValue);
    }
}
