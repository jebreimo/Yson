//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include <filesystem>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include "Yson/YsonDefinitions.hpp"
#include "BinaryReader.hpp"
#include "FromBigEndian.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    class YSON_API UBJsonTokenizer
    {
    public:
        explicit UBJsonTokenizer(std::istream& stream,
                                 const char* buffer = nullptr,
                                 size_t bufferSize = 0);

        explicit UBJsonTokenizer(const std::filesystem::path& fileName);

        UBJsonTokenizer(const char* buffer, size_t bufferSize);

        [[nodiscard]]
        size_t contentSize() const;

        [[nodiscard]]
        UBJsonTokenType contentType() const;

        [[nodiscard]]
        std::string fileName() const;

        bool next();

        bool next(UBJsonTokenType tokenType);

        bool read(void* buffer, size_t size, UBJsonTokenType tokenType);

        bool skip();

        bool skip(UBJsonTokenType tokenType);

        [[nodiscard]]
        size_t position() const;

        [[nodiscard]]
        const void* tokenData() const;

        [[nodiscard]]
        std::string_view token() const;

        [[nodiscard]]
        size_t tokenSize() const;

        [[nodiscard]]
        UBJsonTokenType tokenType() const;

        template <typename T>
        T tokenAs() const
        {
            assert(tokenSize() == sizeof(T));
            T tmp = *static_cast<const T*>(tokenData());
            T result;
            fromBigEndian<sizeof(T)>(reinterpret_cast<char*>(&result),
                                     reinterpret_cast<const char*>(&tmp));
            return result;
        }
    private:
        void readSizedToken();

        void skipSizedToken();

        std::unique_ptr<BinaryReader> m_Reader;
        UBJsonTokenType m_TokenType = {};
        size_t m_ContentSize = 0;
        UBJsonTokenType m_ContentType = {};
        std::string m_FileName;
    };
}
