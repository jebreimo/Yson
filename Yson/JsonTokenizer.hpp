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

namespace Yson
{
    class JsonTokenizer
    {
    public:
        JsonTokenizer();

        bool hasNext() const;
        void next();
        JsonTokenType_t tokenType() const;

        const char* buffer() const;
        void setBuffer(const char* buffer, size_t size);
        size_t bufferSize() const;

        std::string token() const;

        std::pair<const char*, const char*> rawToken() const;

        void reset();
    private:
        JsonTokenType_t nextToken();
        JsonTokenType_t completeCurrentToken();
        JsonTokenType_t completeCommentToken();
        JsonTokenType_t completeStringToken();
        JsonTokenType_t nextCommentToken();
        JsonTokenType_t nextNewlineToken();
        JsonTokenType_t nextStringToken();
        JsonTokenType_t nextValueToken();
        JsonTokenType_t findEndOfBlockComment(bool precededByStar);
        JsonTokenType_t findEndOfBlockString(size_t precedingQuotes, bool escapeFirst);
        JsonTokenType_t findEndOfLineComment();
        JsonTokenType_t findEndOfNewline(bool precededByCr);
        JsonTokenType_t findEndOfString(bool escapeFirst);
        JsonTokenType_t findEndOfValue();
        JsonTokenType_t findEndOfWhitespace();
        bool endsWithEscape() const;
        bool endsWithStar() const;
        size_t countQuotesAtEnd() const;
        void appendTokenToInternalBuffer();
        bool endOfFile() const;

        const char* m_BufferStart;
        const char* m_BufferEnd;
        const char* m_TokenStart;
        const char* m_TokenEnd;
        JsonTokenType_t m_TokenType;
        std::vector<char> m_InternalBuffer;
    };
}
