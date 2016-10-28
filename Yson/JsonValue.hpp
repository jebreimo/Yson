//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "ValueType.hpp"
#include "ReaderException.hpp"

namespace Yson
{
    class JsonValue
    {
    public:
        virtual ~JsonValue() = default;

        virtual ValueType valueType() const = 0;

        virtual const std::string& typeName() const = 0;

        virtual bool value(std::string& value) const;

        virtual bool value(bool& value) const;

        virtual bool value(int8_t& value) const;

        virtual bool value(int16_t& value) const;

        virtual bool value(int32_t& value) const;

        virtual bool value(int64_t& value) const;

        virtual bool value(uint8_t& value) const;

        virtual bool value(uint16_t& value) const;

        virtual bool value(uint32_t& value) const;

        virtual bool value(uint64_t& value) const;

        virtual bool value(float& value) const;

        virtual bool value(double& value) const;

        virtual bool value(long double& value) const;

        virtual bool value(std::vector<uint8_t>& value) const;

        virtual const JsonValue& operator[](size_t index) const;

        virtual const JsonValue& operator[](const std::string& key) const;

        virtual size_t updateStringRef(size_t offset) = 0;
    protected:
        JsonValue();
    private:
        friend class Reader;
    };

    template <typename T>
    T getValue(const JsonValue& token)
    {
        T result;
        if (token.value(result))
            return result;
        YSON_READER_THROW(
                "Cannot convert the token string to the given type.");
    }

    template <typename T>
    T getValue(const JsonValue& token, T&& defaultValue)
    {
        T result;
        if (token.value(result))
            return result;
        return defaultValue;
    }
}
