//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonValue.hpp"
#include "TokenType.hpp"

namespace Yson
{
    class StringRef
    {
    public:
        StringRef()
                : string(nullptr), size()
        {
        }

        StringRef(const char* string, size_t size)
                : string(string), size(size)
        {
        }

        const char* begin() const
        {
            return string;
        }

        const char* end() const
        {
            return string + size;
        }

        const char* string;
        size_t size;
    };

    inline std::string getString(const StringRef& stringRef)
    {
        return std::string(stringRef.string, stringRef.size);
    }

    class JsonSingleValue : public JsonValue
    {
    public:
        JsonSingleValue(StringRef stringRef,
                        const std::shared_ptr<std::string>& buffer,
                        TokenType tokenType);

        ValueType valueType() const override;

        const std::string& typeName() const override;

        bool value(std::string& value) const override;

        bool value(bool& value) const override;

        bool value(int8_t& value) const override;

        bool value(int16_t& value) const override;

        bool value(int32_t& value) const override;

        bool value(int64_t& value) const override;

        bool value(uint8_t& value) const override;

        bool value(uint16_t& value) const override;

        bool value(uint32_t& value) const override;

        bool value(uint64_t& value) const override;

        bool value(float& value) const override;

        bool value(double& value) const override;

        bool value(long double& value) const override;

        bool value(std::vector<uint8_t>& value) const override;

        size_t updateStringRef(size_t offset) override;
    private:
        template <typename T>
        bool parseSignedInteger(T& value) const;

        template <typename T>
        bool parseUnsignedInteger(T& value) const;

        StringRef m_StringRef;
        std::shared_ptr<std::string> m_Buffer;
        TokenType m_TokenType;
    };
}
