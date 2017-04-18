//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
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

        UBJsonReader(char* buffer, size_t bufferSize);

        UBJsonReader(const char* buffer, size_t bufferSize);

        UBJsonReader(const UBJsonReader&) = delete;

        UBJsonReader(UBJsonReader&&);

        ~UBJsonReader() override;

        UBJsonReader& operator=(const UBJsonReader&) = delete;

        UBJsonReader& operator=(UBJsonReader&&);

        bool nextValue() override;

        bool nextKey() override;

        bool nextDocument() override;

        void enter() override;

        void leave() override;

        ValueType valueType(bool analyzeStrings = false) const override;

        DetailedValueType detailedValueType(
                bool analyzeStrings = false) const override;

        bool isOptimizedArray() const;

        std::pair<size_t, DetailedValueType> optimizedArrayProperties() const;

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

        bool readBase64(std::vector<char>& value) const override;

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

        bool readBinary(std::vector<char>& value) override;

        bool readBinary(void* buffer, size_t& size) override;

    private:
        struct Members;
        struct Scope;

        UBJsonReader(Members* members);

        void assertStateIsKeyOrValue() const;

        Scope& currentScope() const;

        template <typename T>
        bool readOptimizedArrayImpl(T* buffer, size_t& size,
                                    UBJsonTokenType tokenType);

        std::unique_ptr<Members> m_Members;
    };
}
