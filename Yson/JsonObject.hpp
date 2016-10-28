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
    class JsonObject : public JsonValue
    {
    public:
        typedef std::pair<std::string, std::unique_ptr<JsonValue>> NamedValue;
        typedef std::vector<NamedValue>::const_iterator Iterator;

        JsonObject(std::vector<NamedValue>&& values);

        ValueType valueType() const override;

        const std::string& typeName() const override;

        bool empty() const;

        size_t size() const;

        const JsonValue& operator[](const std::string& key) const override;

        Iterator begin() const;

        Iterator end() const;

        size_t updateStringRef(size_t offset) override;
    private:
        std::vector<std::pair<std::string, std::unique_ptr<JsonValue>>> m_Values;
    };
}
