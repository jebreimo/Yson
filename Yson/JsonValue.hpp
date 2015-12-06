//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "JsonValueError.hpp"

namespace Yson
{
    class JsonValue;
    typedef std::vector<std::pair<std::string, JsonValue>> JsonObject;
    typedef std::vector<JsonValue> JsonArray;

    class JsonValue
    {
    public:
        enum Type
        {
            OBJECT,
            ARRAY,

            STRING,
            REAL,
            INTEGER,
            BOOLEAN,
            NULL_VALUE
        };

        JsonValue();
        explicit JsonValue(Type type);
        explicit JsonValue(const JsonObject& object);
        explicit JsonValue(const JsonArray& array);
        explicit JsonValue(const std::string& string);
        explicit JsonValue(const char* string);
        explicit JsonValue(double real);
        explicit JsonValue(int32_t integer);
        explicit JsonValue(int64_t integer);
        explicit JsonValue(uint32_t integer);
        explicit JsonValue(uint64_t integer);
        explicit JsonValue(bool boolean);

        JsonValue(const JsonValue& rhs);
        JsonValue(JsonValue&& rhs);

        ~JsonValue();

        JsonValue& operator=(const JsonValue& rhs);
        JsonValue& operator=(JsonValue&& rhs);

        Type type() const;

        const JsonObject& object() const;
        JsonObject& object();

        const JsonArray& array() const;
        JsonArray& array();

        const std::string& string() const;
        double real() const;
        long long integer() const;
        bool boolean() const;
    private:
        union Value
        {
            JsonObject* object;
            JsonArray* array;
            std::string* string;
            double real;
            int64_t integer;
            bool boolean;
        };
        Value m_Value;
        Type m_Type;
    };
}
