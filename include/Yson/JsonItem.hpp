//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include "ValueType.hpp"

namespace Yson
{
    class JsonValue
    {
    public:
    private:
        std::string m_Value;
        ValueType m_Type;
    };

    class JsonArray;
    class JsonObject;

    class JsonItem
    {
    public:
        const JsonItem& operator[](std::string_view) const;
        const JsonItem& operator[](size_t) const;

    private:
        using Data = std::variant<std::shared_ptr<JsonObject>, std::shared_ptr<JsonArray>, JsonValue>;
        Data m_Data;
    };

    class JsonObject
    {
    public:
    private:
        std::deque<std::string> m_Keys;
        std::unordered_map<std::string_view, JsonItem> m_Values;
    };

    class JsonArray
    {
    public:
    private:
        std::vector<JsonItem> m_Values;
    };
}
