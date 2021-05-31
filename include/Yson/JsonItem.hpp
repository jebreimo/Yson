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
#include "JsonValue.hpp"
#include "UBJsonValue.hpp"

namespace Yson
{
    class Array;
    class Object;

    class YSON_API JsonItem
    {
    public:
        using ItemType = std::variant<std::shared_ptr<Object>,
                                      std::shared_ptr<Array>,
                                      JsonValue,
                                      UBJsonValue>;

        explicit JsonItem(ItemType item);

        const JsonItem& operator[](std::string_view) const;

        const JsonItem& operator[](size_t) const;

        [[nodiscard]]
        bool isArray() const;

        [[nodiscard]]
        const Array& array() const;

        [[nodiscard]]
        bool isObject() const;

        [[nodiscard]]
        const Object& object() const;

        [[nodiscard]]
        bool isValue() const;

        [[nodiscard]]
        const Value& value() const;
    private:
        ItemType m_Item;
    };

    template <typename T>
    T get(const JsonItem& item)
    {
        return get<T>(item.value());
    }
}
