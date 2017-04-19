//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "DetailedValueType.hpp"
#include "ValueType.hpp"
#include "YsonDefinitions.hpp"
#include "YsonException.hpp"

namespace Yson
{
    /** @brief Reader is the common interface for JsonReader and UBJsonReader.
      */
    class Reader
    {
    public:
        virtual ~Reader() = default;

        /** @brief Moves to the next key inside the current object.
          *
          * If the returned value is true, then the key can be read with one
          * of the read-functions.
          * @return @arg true another key was found inside the current.
          *         @arg false the Reader has reached the end of the object
          *             without finding any more keys.
          * @throw YsonException if the current location's immediate parent
          *     isn't an object or if the source cannot be parsed.
          */
        virtual bool nextKey() = 0;

        /** @brief Moves to the next value.
          */
        virtual bool nextValue() = 0;

        /** @brief Moves to the next document if the source consists of
          *     mulitiple documents.
          *
          * This function makes it possible to read a file or buffer that
          * consists of multiple JSON or Universal Binary JSON documents.
          */
        virtual bool nextDocument() = 0;

        virtual void enter() = 0;

        virtual void leave() = 0;

        virtual ValueType valueType(bool analyzeStrings = false) const = 0;

        virtual DetailedValueType detailedValueType(
                bool analyzeStrings = false) const = 0;

        virtual bool readNull() const = 0;

        virtual bool read(bool& value) const = 0;

        virtual bool read(int8_t& value) const = 0;

        virtual bool read(int16_t& value) const = 0;

        virtual bool read(int32_t& value) const = 0;

        virtual bool read(int64_t& value) const = 0;

        virtual bool read(uint8_t& value) const = 0;

        virtual bool read(uint16_t& value) const = 0;

        virtual bool read(uint32_t& value) const = 0;

        virtual bool read(uint64_t& value) const = 0;

        virtual bool read(float& value) const = 0;

        virtual bool read(double& value) const = 0;

        virtual bool read(char& value) const = 0;

        virtual bool read(std::string& value) const = 0;

        virtual bool readBinary(void* buffer, size_t& size) = 0;

        virtual bool readBinary(std::vector<char>& value) = 0;

        virtual bool readBase64(std::vector<char>& value) const = 0;
    };

    template <typename T>
    T read(const Reader& reader)
    {
        T value;
        if (reader.read(value))
            return value;
        YSON_THROW("Current value is " + toString(reader.valueType()));
    }
}
