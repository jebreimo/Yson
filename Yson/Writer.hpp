//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <string>
#include "StructureParameters.hpp"

namespace Yson
{
    class Writer
    {
    public:
        virtual ~Writer() = default;

        virtual const std::string& key() const = 0;

        virtual Writer& key(const std::string& key) = 0;

        virtual Writer& beginArray() = 0;

        virtual Writer& beginArray(const StructureParameters& parameters) = 0;

        virtual Writer& endArray() = 0;

        virtual Writer& beginObject() = 0;

        virtual Writer& beginObject(
                const StructureParameters& parameters) = 0;

        virtual Writer& endObject() = 0;

        virtual Writer& null() = 0;

        virtual Writer& boolean(bool value) = 0;

        virtual Writer& value(char value) = 0;

        virtual Writer& value(int8_t value) = 0;

        virtual Writer& value(int16_t value) = 0;

        virtual Writer& value(int32_t value) = 0;

        virtual Writer& value(int64_t value) = 0;

        virtual Writer& value(uint8_t value) = 0;

        virtual Writer& value(uint16_t value) = 0;

        virtual Writer& value(uint32_t value) = 0;

        virtual Writer& value(uint64_t value) = 0;

        virtual Writer& value(float value) = 0;

        virtual Writer& value(double value) = 0;

        virtual Writer& value(const std::string& value) = 0;

        virtual Writer& value(const std::wstring& value) = 0;

        virtual Writer& binary(const void* data, size_t size) = 0;

        virtual Writer& base64(const void* data, size_t size) = 0;

        virtual Writer& flush() = 0;
    };
}
