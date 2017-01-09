//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.12.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Span.hpp"
#include "../TokenType.hpp"

namespace Yson
{
    typedef Span<const char*> TokenSpan;

    struct Result
    {
        Result(TokenType tokenType, const char* endOfToken)
            : tokenType(tokenType),
              endOfToken(endOfToken),
              isIncomplete(false)
        {}

        Result(TokenType tokenType,
               const char* endOfToken,
               bool isIncomplete)
            : tokenType(tokenType),
              endOfToken(endOfToken),
              isIncomplete(isIncomplete)
        {}

        TokenType tokenType;
        const char* endOfToken;
        bool isIncomplete;
    };

    Result nextToken(TokenSpan span);

    Result completeToken(TokenSpan firstPart, TokenSpan secondPart,
                         bool isEndOfFile);

    Result completeCommentToken(TokenSpan firstPart,
                                TokenSpan secondPart,
                                bool isEndOfFile);

    Result completeStringToken(TokenSpan firstPart,
                               TokenSpan secondPart, bool isEndOfFile);

    Result nextCommentToken(TokenSpan span);

    Result nextStringToken(TokenSpan span);

    Result findEndOfBlockComment(TokenSpan span, bool precededByStar,
                                 bool isEndOfFile);

    Result findEndOfBlockString(TokenSpan span, size_t precedingQuotes,
                                bool escapeFirst, bool isEndOfFile);

    Result findEndOfLineComment(TokenSpan span, bool isEndOfFile);

    Result findEndOfNewline(TokenSpan span, bool precededByCr,
                            bool isEndOfFile);

    Result findEndOfString(TokenSpan span, bool escapeFirst,
                           bool isEndOfFile);

    Result findEndOfValue(TokenSpan span, bool isEndOfFile);

    Result findEndOfWhitespace(TokenSpan span, bool isEndOfFile);

    bool endsWithEscape(TokenSpan span);

    bool endsWithStar(TokenSpan span);

    size_t countQuotesAtEnd(TokenSpan span);

    TokenType determineCommentType(TokenSpan span,
                                   bool precededBySlash);

    std::pair<TokenType, bool> determineStringType(
            TokenSpan span,
            size_t precedingQuotes,
            bool isEndOfFile);
}
