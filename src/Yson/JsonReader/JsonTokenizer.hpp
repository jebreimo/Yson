//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include <string_view>
#include "Yson/YsonDefinitions.hpp"
#include "JsonTokenType.hpp"
#include "TextReader.hpp"

namespace Yson
{
    class YSON_API JsonTokenizer
    {
    public:
        JsonTokenizer(std::istream& stream,
                      const char* buffer = nullptr,
                      size_t bufferSize = 0);

        JsonTokenizer(const std::string& fileName);

        JsonTokenizer(const char* buffer, size_t bufferSize);

        ~JsonTokenizer();

        JsonTokenizer(JsonTokenizer&&) noexcept;

        bool next();

        JsonTokenType tokenType() const;

        std::string_view token() const;

        std::string tokenString() const;

        const std::string& fileName() const;

        size_t lineNumber() const;

        size_t columnNumber() const;

        size_t chunkSize() const;

        void setChunkSize(size_t value);
    private:
        bool internalNext();

        bool fillBuffer();

        void removeLineContinuations();

        std::unique_ptr<TextReader> m_TextReader;
        std::string m_FileName;
        std::string m_Buffer;
        const char* m_BufferStart = nullptr;
        const char* m_BufferEnd = nullptr;
        const char* m_TokenStart = nullptr;
        const char* m_TokenEnd = nullptr;
        const char* m_NextToken = nullptr;
        size_t m_LineNumber = 1;
        size_t m_ColumnNumber = 1;
        JsonTokenType m_TokenType = JsonTokenType::INVALID_TOKEN;
        size_t m_ChunkSize;
    };
}
