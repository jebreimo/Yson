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

namespace Yson
{
    class JsonArray;
    class JsonObject;

    class JsonItem
    {
    public:
        using InternalItem = std::variant<std::shared_ptr<JsonObject>,
            std::shared_ptr<JsonArray>,
            JsonValue>;

        explicit JsonItem(InternalItem item);

        const JsonItem& operator[](std::string_view) const;

        const JsonItem& operator[](size_t) const;

        [[nodiscard]]
        bool isArray() const;

        [[nodiscard]]
        const JsonArray& array() const;

        [[nodiscard]]
        bool isObject() const;

        [[nodiscard]]
        const JsonObject& object() const;

        [[nodiscard]]
        bool isValue() const;

        [[nodiscard]]
        const JsonValue& value() const;
    private:
        InternalItem m_Item;
    };

    template <typename T>
    T get(const JsonItem& item)
    {
        return get<T>(item.value());
    }
}
