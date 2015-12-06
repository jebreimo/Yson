//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonValue.hpp"

#include <cassert>
#include <cmath>

namespace Yson
{
    JsonValue::JsonValue()
        : m_Type(NULL_VALUE)
    {
        m_Value.integer = 0;
    }

    JsonValue::JsonValue(Type type)
        : m_Type(type)
    {
        switch (type)
        {
        case OBJECT:
            m_Value.object = new JsonObject;
            break;
        case ARRAY:
            m_Value.array = new JsonArray;
            break;
        case STRING:
            m_Value.string = new std::string;
            break;
        case REAL:
            m_Value.real = 0;
            break;
        case INTEGER:
        case NULL_VALUE:
            m_Value.integer = 0;
            break;
        case BOOLEAN:
            m_Value.boolean = false;
            break;
        }
    }

    JsonValue::JsonValue(const JsonObject& object)
        : m_Type(OBJECT)
    {
        m_Value.object = new JsonObject;
    }

    JsonValue::JsonValue(const JsonArray& array)
        : m_Type(ARRAY)
    {
        m_Value.array = new JsonArray;
    }

    JsonValue::JsonValue(const std::string& string)
        : m_Type(STRING)
    {
        m_Value.string = new std::string(string);
    }

    JsonValue::JsonValue(const char* string)
        : m_Type(STRING)
    {
        m_Value.string = new std::string(string);
    }

    JsonValue::JsonValue(double real)
        : m_Type(REAL)
    {
        m_Value.real = real;
    }

    JsonValue::JsonValue(int32_t integer)
        : m_Type(INTEGER)
    {
        m_Value.integer = (int64_t)integer;
    }

    JsonValue::JsonValue(int64_t integer)
        : m_Type(INTEGER)
    {
        m_Value.integer = (int64_t)integer;
    }

    JsonValue::JsonValue(uint32_t integer)
        : m_Type(INTEGER)
    {
        m_Value.integer = (int64_t)integer;
    }

    JsonValue::JsonValue(uint64_t integer)
        : m_Type(INTEGER)
    {
        m_Value.integer = (int64_t)integer;
    }

    JsonValue::JsonValue(bool boolean)
        : m_Type(BOOLEAN)
    {
        m_Value.boolean = boolean;
    }

    JsonValue::JsonValue(const JsonValue& rhs)
        : m_Type(rhs.m_Type)
    {
        static_assert(sizeof(Value) == sizeof(int64_t),
                      "JsonValue requires that doubles and pointers are 64-bit or less.");
        switch (m_Type)
        {
        case OBJECT:
            m_Value.object = new JsonObject(rhs.object());
            break;
        case ARRAY:
            m_Value.array = new JsonArray(rhs.array());
            break;
        case STRING:
            m_Value.string = new std::string(rhs.string());
            break;
        default:
            m_Value.integer = rhs.m_Value.integer;
            break;
        }
    }

    JsonValue::JsonValue(JsonValue&& rhs)
        : m_Type(rhs.m_Type)
    {
        static_assert(sizeof(Value) == sizeof(int64_t),
                      "JsonValue requires that doubles and pointers are 64-bit or less.");
        m_Value.integer = rhs.m_Value.integer;
        rhs.m_Type = NULL_VALUE;
        rhs.m_Value.object = nullptr;
    }

    JsonValue::~JsonValue()
    {
        switch (m_Type)
        {
        case OBJECT: delete m_Value.object; break;
        case ARRAY: delete m_Value.array; break;
        case STRING: delete m_Value.string; break;
        default: break;
        }
    }

    JsonValue& JsonValue::operator=(const JsonValue& rhs)
    {
        static_assert(sizeof(Value) == sizeof(int64_t),
                      "JsonValue requires that doubles and pointers are 64-bit or less.");
        JsonValue copy(rhs); // Use copy constructor
        std::swap(m_Value.integer, copy.m_Value.integer);
        std::swap(m_Type, copy.m_Type);
        return *this; // Use move assignment operator
    }

    JsonValue& JsonValue::operator=(JsonValue&& rhs)
    {
        static_assert(sizeof(Value) == sizeof(int64_t), "JsonValue requires that doubles and pointers are 64-bit or less.");
        m_Type = rhs.m_Type;
        m_Value.integer = rhs.m_Value.integer;
        rhs.m_Value.object = nullptr;
        rhs.m_Type = NULL_VALUE;
        return *this;
    }

    JsonValue::Type JsonValue::type() const
    {
        return m_Type;
    }

    const JsonObject& JsonValue::object() const
    {
        if (m_Type != OBJECT)
            throw JsonValueError("This JsonValue doesn't contain an object.");
        return *m_Value.object;
    }

    JsonObject& JsonValue::object()
    {
        if (m_Type != OBJECT)
            throw JsonValueError("This JsonValue doesn't contain an object.");
        return *m_Value.object;
    }

    const JsonArray& JsonValue::array() const
    {
        if (m_Type != ARRAY)
            throw JsonValueError("This JsonValue doesn't contain an array.");
        return *m_Value.array;
    }

    JsonArray& JsonValue::array()
    {
        if (m_Type != ARRAY)
            throw JsonValueError("This JsonValue doesn't contain an array.");
        return *m_Value.array;
    }

    const std::string& JsonValue::string() const
    {
        if (m_Type != STRING)
            throw JsonValueError("This JsonValue doesn't contain an string.");
        return *m_Value.string;
    }

    double JsonValue::real() const
    {
        if (m_Type == REAL)
            return m_Value.real;
        else if (m_Type == INTEGER)
            return (double)m_Value.integer;
        else
            throw JsonValueError("This JsonValue doesn't contain a real.");
    }

    int64_t JsonValue::integer() const
    {
        if (m_Type == INTEGER)
        {
            return m_Value.integer;
        }
        else if (m_Type == REAL)
        {
            double i;
            double f = std::modf(m_Value.real, &i);
            if (f == 0 && i >= (double)std::numeric_limits<int64_t>::min() &&
                    i <= (double)std::numeric_limits<int64_t>::max())
                return (int64_t)i;
        }

        throw JsonValueError("This JsonValue doesn't contain an integer.");
    }

    bool JsonValue::boolean() const
    {
        if (m_Type != BOOLEAN)
            throw JsonValueError("This JsonValue doesn't contain a boolean.");
        return false;
    }
}
