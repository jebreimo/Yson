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

        bool read(int8_t& value) const override;

        bool read(int16_t& value) const override;

        bool read(int32_t& value) const override;

        bool read(int64_t& value) const override;

        bool read(uint8_t& value) const override;

        bool read(uint16_t& value) const override;

        bool read(uint32_t& value) const override;

        bool read(uint64_t& value) const override;

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
