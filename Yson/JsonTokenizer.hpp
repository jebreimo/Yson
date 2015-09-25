//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include <vector>

namespace Yson
{
    class JsonTokenizer
    {
    public:
        enum TokenType
        {
            INVALID_TOKEN,
            INVALID_CHARACTER,
            ARRAY_START,
            ARRAY_END,
            OBJECT_START,
            OBJECT_END,
            COLON,
            COMMA,
            STRING,
            VALUE,
            END_OF_BUFFER,
            BLOCK_STRING,
            COMMENT, ///< A line comment, i.e. one that starts with //.
            BLOCK_COMMENT, ///< A comment that starts with /* and ends with */.
            WHITESPACE,
            NEWLINE
        };

        JsonTokenizer();

        bool hasNext() const;
        void next();
        TokenType tokenType() const;

        const char* buffer() const;
        void setBuffer(const char* buffer, size_t size);
        size_t bufferSize() const;

        std::string token() const;
        void getToken(const char*& start, const char*& end) const;

        void reset();
    private:
        TokenType nextToken();
        TokenType completeCurrentToken();
        TokenType completeCommentToken();
        TokenType completeStringToken();
        TokenType nextCommentToken();
        TokenType nextNewlineToken();
        TokenType nextStringToken();
        TokenType nextValueToken();
        TokenType findEndOfBlockComment(bool precededByStar);
        TokenType findEndOfBlockString(size_t precedingQuotes, bool escapeFirst);
        TokenType findEndOfLineComment();
        TokenType findEndOfNewline(bool precededByCr);
        TokenType findEndOfString(bool escapeFirst);
        TokenType findEndOfValue();
        TokenType findEndOfWhitespace();
        bool endsWithEscape() const;
        bool endsWithStar() const;
        size_t countQuotesAtEnd() const;
        void appendTokenToInternalBuffer();
        bool endOfFile() const;

        const char* m_BufferStart;
        const char* m_BufferEnd;
        const char* m_TokenStart;
        const char* m_TokenEnd;
        TokenType m_TokenType;
        // size_t m_LineNo;
        // size_t m_ColumnNo;
        std::vector<char> m_InternalBuffer;
    };
}
