//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include "../YsonDefinitions.hpp"
#include "../Common/StringView.hpp"
#include "BinaryReader.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    class YSON_API UBJsonTokenizer
    {
    public:
        UBJsonTokenizer(std::istream& stream,
                        const char* buffer = nullptr,
                        size_t bufferSize = 0);

        UBJsonTokenizer(const std::string& fileName);

        UBJsonTokenizer(char* buffer, size_t bufferSize);

        UBJsonTokenizer(const char* buffer, size_t bufferSize);

        size_t contentSize() const;

        UBJsonTokenType contentType() const;

        std::string fileName() const;

        bool next();

        bool next(UBJsonTokenType tokenType);

        bool read(void* buffer, size_t size, UBJsonTokenType tokenType);

        size_t position() const;

        const void* tokenData() const;

        std::string_view token() const;

        size_t tokenSize() const;

        UBJsonTokenType tokenType() const;

        template <typename T>
        T tokenAs() const
        {
            return *static_cast<const T*>(tokenData());
        }
    private:
        void readSizedToken();

        std::unique_ptr<BinaryReader> m_Reader;
        UBJsonTokenType m_TokenType;
        size_t m_ContentSize = 0;
        UBJsonTokenType m_ContentType;
        std::string m_FileName;
    };
}
