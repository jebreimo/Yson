//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "JsonItem.hpp"
#include "Reader.hpp"

namespace Yson
{
    enum class UBJsonTokenType : char;

    class YSON_API UBJsonReader : public Reader
    {
    public:
        UBJsonReader();

        explicit UBJsonReader(std::istream& stream);

        explicit UBJsonReader(const std::string& fileName);

        UBJsonReader(const char* buffer, size_t bufferSize);

        UBJsonReader(std::istream& stream,
                     const char* buffer,
                     size_t bufferSize);

        UBJsonReader(const UBJsonReader&) = delete;

        UBJsonReader(UBJsonReader&&) noexcept;

        ~UBJsonReader() override;

        UBJsonReader& operator=(const UBJsonReader&) = delete;

        UBJsonReader& operator=(UBJsonReader&&) noexcept;

        bool nextValue() override;

        bool nextKey() override;

        bool nextDocument() override;

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
        bool isOptimizedArray() const;

        [[nodiscard]]
        std::pair<size_t, DetailedValueType> optimizedArrayProperties() const;

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

        bool read(std::string& value) const override;

        bool read(char& value) const override;

        bool read(float& value) const override;

        bool read(double& value) const override;

        bool read(long double& value) const;

        bool readOptimizedArray(int8_t* buffer, size_t& size);

        bool readOptimizedArray(int16_t* buffer, size_t& size);

        bool readOptimizedArray(int32_t* buffer, size_t& size);

        bool readOptimizedArray(int64_t* buffer, size_t& size);

        bool readOptimizedArray(uint8_t* buffer, size_t& size);

        bool readOptimizedArray(float* buffer, size_t& size);

        bool readOptimizedArray(double* buffer, size_t& size);

        bool readOptimizedArray(char* buffer, size_t& size);

        bool readBase64(std::vector<char>& value) const override;

        bool readBinary(std::vector<char>& value) override;

        bool readBinary(void* buffer, size_t& size) override;

        [[nodiscard]]
        JsonItem readCurrentItem() override;

        [[nodiscard]]
        std::string fileName() const override;

        [[nodiscard]]
        size_t lineNumber() const override;

        [[nodiscard]]
        size_t columnNumber() const override;

        ReaderState state() const override;

        std::string scope() const override;

        bool isExpandOptimizedByteArraysEnabled() const;

        UBJsonReader& setExpandOptimizedByteArraysEnabled(bool value);
    private:
        struct Members;
        struct Scope;

        explicit UBJsonReader(Members* members);

        void assertStateIsKeyOrValue() const;

        [[nodiscard]]
        Scope& currentScope() const;

        [[nodiscard]]
        JsonItem readArray(bool expandOptmizedByteArrays);

        [[nodiscard]]
        JsonItem readObject(bool expandOptmizedByteArrays);

        template <typename T>
        bool readOptimizedArrayImpl(T* buffer, size_t& size,
                                    UBJsonTokenType tokenType);

        std::unique_ptr<Members> m_Members;
    };
}
