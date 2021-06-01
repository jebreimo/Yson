//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ValueItem.hpp"

namespace Yson
{
    enum class UBJsonTokenType : char;

    class YSON_API UBJsonValueItem : public ValueItem
    {
    public:
        UBJsonValueItem(std::string value, UBJsonTokenType type);

        [[nodiscard]]
        ValueType valueType() const override;

        [[nodiscard]]
        ValueType valueType(bool analyzeStrings) const override;

        [[nodiscard]]
        bool isNull() const override;

        bool get(bool& value) const override;

        bool get(int8_t& value) const override;

        bool get(int16_t& value) const override;

        bool get(int32_t& value) const override;

        bool get(int64_t& value) const override;

        bool get(uint8_t& value) const override;

        bool get(uint16_t& value) const override;

        bool get(uint32_t& value) const override;

        bool get(uint64_t& value) const override;

        bool get(float& value) const override;

        bool get(double& value) const override;

        bool get(long double& value) const override;

        bool get(char& value) const override;

        bool get(std::string& value) const override;

        bool getBase64(std::vector<char>& value) const override;

        bool getBinary(std::vector<char>& value) const override;

        bool getBinary(void* buffer, size_t& size) const override;

    private:
        std::string m_Value;
        UBJsonTokenType m_Type;
    };
}
