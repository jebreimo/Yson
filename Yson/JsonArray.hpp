//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonValue.hpp"

namespace Yson
{
    class JsonArray : public JsonValue
    {
    public:
        JsonArray(std::vector<std::unique_ptr<JsonValue>>&& values);

        ValueType valueType() const override;

        const std::string& typeName() const override;

        bool empty() const;

        size_t size() const;

        const JsonValue& operator[](size_t index) const override;

        std::vector<std::unique_ptr<JsonValue>>::const_iterator begin() const;

        std::vector<std::unique_ptr<JsonValue>>::const_iterator end() const;

        size_t updateStringRef(size_t offset) override;
    private:
        std::vector<std::unique_ptr<JsonValue>> m_Values;
    };
}
