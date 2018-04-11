//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include <stack>
#include <vector>
#include "Writer.hpp"
#include "YsonDefinitions.hpp"

namespace Yson
{
    class YSON_API UBJsonWriter : public Writer
    {
    public:
        UBJsonWriter();

        UBJsonWriter(const std::string& fileName);

        UBJsonWriter(std::ostream& stream);

        UBJsonWriter(const UBJsonWriter&) = delete;

        UBJsonWriter(UBJsonWriter&&) noexcept;

        ~UBJsonWriter() override;

        UBJsonWriter& operator=(const UBJsonWriter&) = delete;

        UBJsonWriter& operator=(UBJsonWriter&&) noexcept;

        std::ostream& stream();

        const std::string& key() const override;

        UBJsonWriter& key(const std::string& key) override;

        UBJsonWriter& beginArray() override;

        UBJsonWriter& endArray() override;

        UBJsonWriter& beginArray(const StructureParameters& params) override;

        UBJsonWriter& beginObject() override;

        UBJsonWriter& beginObject(
                const StructureParameters& parameters) override;

        UBJsonWriter& endObject() override;

        UBJsonWriter& null() override;

        UBJsonWriter& boolean(bool value) override;

        UBJsonWriter& value(int8_t value) override;

        UBJsonWriter& value(int16_t value) override;

        UBJsonWriter& value(int32_t value) override;

        UBJsonWriter& value(int64_t value) override;

        UBJsonWriter& value(uint8_t value) override;

        UBJsonWriter& value(uint16_t value) override;

        UBJsonWriter& value(uint32_t value) override;

        UBJsonWriter& value(uint64_t value) override;

        UBJsonWriter& value(char value) override;

        UBJsonWriter& value(double value) override;

        UBJsonWriter& value(float value) override;

        UBJsonWriter& value(const std::string& text) override;

        UBJsonWriter& value(const std::wstring& text) override;

        UBJsonWriter& binary(const void* data, size_t size) override;

        UBJsonWriter& base64(const void* data, size_t size) override;

        UBJsonWriter& noop();

        bool isStrictIntegerSizesEnabled() const;

        UBJsonWriter& setStrictIntegerSizesEnabled(bool value);

        UBJsonWriter& flush() override;
    private:
        UBJsonWriter(std::unique_ptr<std::ostream> streamPtr,
                     std::ostream* stream);

        UBJsonWriter& beginStructure(UBJsonValueType structureType,
                                     const UBJsonParameters& parameters);

        UBJsonWriter& endStructure(UBJsonValueType structureType);

        void beginValue();

        template <typename T>
        UBJsonWriter& writeInteger(T value);

        template <typename T>
        UBJsonWriter& writeFloat(T value);

        struct Members;
        std::unique_ptr<Members> m_Members;
    };
}
