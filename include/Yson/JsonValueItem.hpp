//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ValueItem.hpp"

namespace Yson
{
    enum class JsonTokenType;

    /**
     * @brief A class holding JSON values, as opposed to objects and arrays.
     *
     * This is one of the four types of items that are the constituents of
     * JsonItem.
     */
    class YSON_API JsonValueItem : public ValueItem
    {
    public:
        JsonValueItem(std::string value, JsonTokenType tokenType);

        [[nodiscard]]
        ValueType valueType() const final;

        [[nodiscard]]
        ValueType valueType(bool analyzeStrings) const final;

        [[nodiscard]]
        bool isNull() const final;

        bool get(bool& value) const final;

        bool get(int8_t& value) const final;

        bool get(int16_t& value) const final;

        bool get(int32_t& value) const final;

        bool get(int64_t& value) const final;

        bool get(uint8_t& value) const final;

        bool get(uint16_t& value) const final;

        bool get(uint32_t& value) const final;

        bool get(uint64_t& value) const final;

        bool get(float& value) const final;

        bool get(double& value) const final;

        bool get(long double& value) const final;

        bool get(char& value) const final;

        bool get(std::string& value) const final;

        bool getBase64(std::vector<char>& value) const final;

        bool getBinary(std::vector<char>& value) const final;

        bool getBinary(void* buffer, size_t& size) const final;
    private:
        std::string m_Value;
        JsonTokenType m_Type;
    };
}
