//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include <vector>
#include "JsonTokenType.hpp"
#include "YsonDefinitions.hpp"

namespace Yson
{
    class YSON_API JsonTokenizer
    {
    public:
        JsonTokenizer();
        ~JsonTokenizer();

        bool hasNext() const;
        void next();
        std::pair<JsonTokenType_t, bool> peek() const;
        JsonTokenType_t tokenType() const;

        const char* buffer() const;
        void setBuffer(const char* buffer, size_t size);
        size_t bufferSize() const;

        std::string token() const;

        std::pair<const char*, const char*> rawToken() const;

        void reset();
    private:
        void appendTokenToInternalBuffer();
        bool isEndOfFile() const;

        const char* m_BufferStart;
        const char* m_BufferEnd;
        const char* m_TokenStart;
        const char* m_TokenEnd;
        JsonTokenType_t m_TokenType;
        std::vector<char> m_InternalBuffer;
    };
}
