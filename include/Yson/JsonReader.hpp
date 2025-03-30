//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "Reader.hpp"

#include <iosfwd>
#include <memory>
#include "JsonItem.hpp"

namespace Yson
{
    /**
     * @brief A class for reading JSON data.
     *
     * Provides a recursive iterator for reading JSON data. With `nextKey`and
     * `nextValue` you can iterate over the keys and values of an object or
     * array. With `enter` and `leave` you can enter and leave objects and
     * arrays.
     *
     * In addition to JSON itself, JsonReader supports parts of JSON5,
     * including comments, unquoted keys, and special floating point values.
     */
    class YSON_API JsonReader : public Reader
    {
    public:
        JsonReader();

        explicit JsonReader(std::istream& stream);

        explicit JsonReader(const std::string& fileName);

        JsonReader(const char* buffer, size_t bufferSize);

        JsonReader(std::istream& stream,
                   const char* buffer,
                   size_t bufferSize);

        JsonReader(const JsonReader&) = delete;

        JsonReader(JsonReader&&) noexcept;

        ~JsonReader() override;

        JsonReader& operator=(const JsonReader&) = delete;

        JsonReader& operator=(JsonReader&&) noexcept;

        bool nextDocument() override;

        bool nextKey() override;

        bool nextValue() override;

        void enter() override;

        void leave() override;

        [[nodiscard]]
        ValueType valueType() const override;

        [[nodiscard]]
        ValueType valueType(bool analyzeStrings) const override;

        [[nodiscard]]
        DetailedValueType detailedValueType() const override;

        [[nodiscard]]
        DetailedValueType detailedValueType(
                bool analyzeStrings) const override;

        [[nodiscard]]
        bool readNull() const override;

        bool read(bool& value) const override;

        bool read(signed char& value) const override;

        bool read(short& value) const override;

        bool read(int& value) const override;

        bool read(long& value) const override;

        bool read(long long& value) const override;

        bool read(unsigned char& value) const override;

        bool read(unsigned short& value) const override;

        bool read(unsigned int& value) const override;

        bool read(unsigned long& value) const override;

        bool read(unsigned long long& value) const override;

        bool read(float& value) const override;

        bool read(double& value) const override;

        bool read(long double& value) const;

        bool read(char& value) const override;

        bool read(std::string& value) const override;

        bool readBase64(std::vector<char>& value) const override;

        bool readBinary(std::vector<char>& value) override;

        bool readBinary(void* buffer, size_t& size) override;

        JsonItem readItem() override;

        [[nodiscard]]
        std::string fileName() const override;

        [[nodiscard]]
        size_t lineNumber() const override;

        [[nodiscard]]
        size_t columnNumber() const override;

        [[nodiscard]]
        ReaderState state() const override;

        [[nodiscard]]
        std::string scope() const override;
    private:
        void assertStateIsKeyOrValue() const;

        [[nodiscard]]
        bool currentTokenIsValueOrString() const;

        [[nodiscard]]
        bool currentTokenIsString() const;

        [[nodiscard]]
        bool currentTokenIsValue() const;

        [[nodiscard]]
        JsonItem readArray();

        [[nodiscard]]
        JsonItem readObject();

        template <typename T>
        bool readInteger(T& value) const;

        template <typename T>
        bool readFloatingPoint(T& value) const;

        struct Members;
        std::unique_ptr<Members> m_Members;
    };
}
