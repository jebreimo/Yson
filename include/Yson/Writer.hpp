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
#include <string_view>
#include "StructureParameters.hpp"

namespace Yson
{
    class Writer
    {
    public:
        virtual ~Writer() = default;

        virtual std::ostream* stream() = 0;

        /**
         * @brief Returns the writer's internal buffer.
         *
         * The buffer is only available if the writer isn't associated with
         * a stream (i.e. stream() returns nullptr). If the writer is
         * associated with a stream, this function returns nullptr and 0.
         *
         * @return A pointer to the buffer and the buffer's size.
         */
        virtual std::pair<const void*, size_t> buffer() const = 0;

        virtual const std::string& key() const = 0;

        /**
         * @brief Sets the key (or name) of the next value in an object.
         *
         * The key is ignored if the current structure isn't an object (i.e.
         * array or at the start of the start of the document).
         *
         * @param key the value name. Non-JSON characters in @a key are
         *      automatically escaped.
         * @return A reference to the instance.
         */
        virtual Writer& key(std::string key) = 0;

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

        virtual Writer& value(std::string_view value) = 0;

        virtual Writer& value(std::wstring_view value) = 0;

        virtual Writer& binary(const void* data, size_t size) = 0;

        virtual Writer& base64(const void* data, size_t size) = 0;

        virtual Writer& flush() = 0;
    };
}
