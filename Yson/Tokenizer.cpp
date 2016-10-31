//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Tokenizer.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include "Detail/Span.hpp"

namespace Yson
{
    using namespace std::placeholders;

    typedef Span<const char*> TokenSpan;

    namespace
    {
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

    Tokenizer::Tokenizer()
        : m_BufferStart(nullptr),
          m_BufferEnd(nullptr),
          m_TokenStart(nullptr),
          m_TokenEnd(nullptr),
          m_TokenType(TokenType::INVALID_TOKEN)
    {}

    Tokenizer::~Tokenizer()
    {}

    bool Tokenizer::hasNext() const
    {
        return !isEndOfFile() || !m_InternalBuffer.empty();
    }

    void Tokenizer::next()
    {
        m_TokenStart = m_TokenEnd;
        if (m_InternalBuffer.empty()
            || m_TokenType != TokenType::END_OF_BUFFER)
        {
            m_InternalBuffer.clear();
            auto result = nextToken(makeSpan(m_TokenStart, m_BufferEnd));
            m_TokenEnd = result.endOfToken;
            if (!result.isIncomplete)
            {
                m_TokenType = result.tokenType;
            }
            else
            {
                m_TokenType = TokenType::END_OF_BUFFER;
                appendTokenToInternalBuffer();
            }
        }
        else
        {
            auto result = completeToken(
                    TokenSpan(m_InternalBuffer.data(),
                               m_InternalBuffer.data()
                               + m_InternalBuffer.size()),
                    makeSpan(m_BufferStart, m_BufferEnd),
                    isEndOfFile());
            m_TokenType = result.isIncomplete ? TokenType::END_OF_BUFFER
                                              : result.tokenType;
            m_TokenEnd = result.endOfToken;
            appendTokenToInternalBuffer();
        }
    }

    std::pair<TokenType, bool> Tokenizer::peek() const
    {
        if (!m_InternalBuffer.empty()
            && m_TokenType == TokenType::END_OF_BUFFER)
        {
            auto result = completeToken(
                    TokenSpan(m_InternalBuffer.data(),
                              m_InternalBuffer.data()
                              + m_InternalBuffer.size()),
                    makeSpan(m_TokenEnd, m_BufferEnd),
                    isEndOfFile());
            return std::make_pair(result.tokenType, !result.isIncomplete);
        }
        else
        {
            auto result = nextToken(makeSpan(m_TokenEnd, m_BufferEnd));
            return std::make_pair(result.tokenType, !result.isIncomplete);
        }
    }

    TokenType Tokenizer::tokenType() const
    {
        return m_TokenType;
    }

    const char* Tokenizer::buffer() const
    {
        return m_BufferStart;
    }

    void Tokenizer::setBuffer(const char* buffer, size_t size)
    {
        m_BufferStart = m_TokenStart = m_TokenEnd = buffer;
        m_BufferEnd = buffer + size;
    }

    size_t Tokenizer::bufferSize() const
    {
        return size_t(m_BufferEnd - m_BufferStart);
    }

    size_t Tokenizer::currentPosition() const
    {
        return size_t(m_TokenEnd - m_BufferStart);
    }

    std::string Tokenizer::token() const
    {
        auto span = rawToken();
        return std::string(span.first, span.second);
    }

    std::pair<const char*, const char*> Tokenizer::rawToken() const
    {
        if (m_InternalBuffer.empty())
        {
            return std::make_pair(m_TokenStart, m_TokenEnd);
        }
        else
        {
            return std::make_pair(
                    m_InternalBuffer.data(),
                    m_InternalBuffer.data() + m_InternalBuffer.size());
        }
    }

    void Tokenizer::reset()
    {
        m_TokenStart = m_TokenEnd = m_BufferStart;
        m_TokenType = TokenType::INVALID_TOKEN;
        m_InternalBuffer.clear();
    }

    void Tokenizer::appendTokenToInternalBuffer()
    {
        size_t oldSize = m_InternalBuffer.size();
        size_t size = oldSize + (size_t)(m_TokenEnd - m_TokenStart);
        if (size != m_InternalBuffer.size())
        {
          m_InternalBuffer.resize(size);
          char* dst = &m_InternalBuffer[oldSize];
          std::copy(m_TokenStart, m_TokenEnd, dst);
        }
    }

    bool Tokenizer::isEndOfFile() const
    {
        return m_BufferStart == m_BufferEnd;
    }

    namespace
    {
        Result completeToken(TokenSpan firstPart, TokenSpan secondPart,
                             bool isEndOfFile)
        {
            switch (firstPart.front())
            {
            case '"':
                return completeStringToken(firstPart, secondPart,
                                           isEndOfFile);
            case '\r':
                return findEndOfNewline(secondPart, true, isEndOfFile);
            case '/':
                return completeCommentToken(firstPart, secondPart,
                                            isEndOfFile);
            case ' ':
            case '\t':
                return findEndOfWhitespace(secondPart, isEndOfFile);
            default:
                return findEndOfValue(secondPart, isEndOfFile);
            }
        }

        Result completeCommentToken(TokenSpan firstPart,
                                    TokenSpan secondPart,
                                    bool isEndOfFile)
        {
            auto tokenType = determineCommentType(firstPart, false);
            if (tokenType == TokenType::END_OF_BUFFER)
            {
                if (isEndOfFile)
                {
                    return Result(TokenType::INVALID_TOKEN,
                                  secondPart.end());
                }
                tokenType = determineCommentType(secondPart, true);
                if (tokenType == TokenType::COMMENT
                    || tokenType == TokenType::BLOCK_COMMENT)
                {
                    ++secondPart.begin();
                }
            }

            if (tokenType == TokenType::COMMENT)
                return findEndOfLineComment(secondPart, isEndOfFile);
            if (tokenType == TokenType::BLOCK_COMMENT)
                return findEndOfBlockComment(secondPart,
                                             endsWithStar(firstPart),
                                             isEndOfFile);

            auto result = findEndOfValue(secondPart, isEndOfFile);
            result.tokenType = TokenType::INVALID_TOKEN;
            return result;
        }

        Result completeStringToken(TokenSpan firstPart, TokenSpan secondPart,
                                   bool isEndOfFile)
        {
            auto tokenType = determineStringType(firstPart, 0, false);
            if (!tokenType.second)
            {
                tokenType = determineStringType(secondPart,
                                                firstPart.size(),
                                                isEndOfFile);
                if (tokenType.first == TokenType::STRING && isEndOfFile)
                    return Result(TokenType::STRING, secondPart.end());
                if (tokenType.first == TokenType::BLOCK_STRING)
                    secondPart.begin() += 3 - firstPart.size();
            }

            if (tokenType.first == TokenType::STRING)
            {
                return findEndOfString(secondPart, endsWithEscape(firstPart),
                                       isEndOfFile);
            }
            else if (tokenType.first != TokenType::BLOCK_STRING)
            {
                return Result(tokenType.first, secondPart.begin());
            }
            else if (firstPart.size() >= 3)
            {
                size_t quotes = countQuotesAtEnd(firstPart);
                if (quotes == firstPart.size())
                    quotes -= 3;
                return findEndOfBlockString(
                        secondPart, quotes,
                        quotes == 0 && endsWithEscape(firstPart),
                        isEndOfFile);
            }
            else
            {
                return findEndOfBlockString(secondPart, 0, false,
                                            isEndOfFile);
            }
        }

        Result nextToken(TokenSpan span)
        {
            if (isEmpty(span))
                return Result(TokenType::END_OF_BUFFER, span.end());
            switch (span.front())
            {
            case ' ':
            case '\t':
                return findEndOfWhitespace(span, false);
            case '\r':
            case '\n':
                return findEndOfNewline(span, false, false);
            case '[':
                return Result(TokenType::START_ARRAY, span.begin() + 1);
            case ']':
                return Result(TokenType::END_ARRAY, span.begin() + 1);
            case '{':
                return Result(TokenType::START_OBJECT, span.begin() + 1);
            case '}':
                return Result(TokenType::END_OBJECT, span.begin() + 1);
            case ':':
                return Result(TokenType::COLON, span.begin() + 1);
            case ',':
                return Result(TokenType::COMMA, span.begin() + 1);
            case '"':
                return nextStringToken(span);
            case '/':
                return nextCommentToken(span);
            default:
                return findEndOfValue(span, false);
            }
        }

        Result nextCommentToken(TokenSpan span)
        {
            assert(!isEmpty(span));
            assert(span.front() == '/');

            auto tokenType = determineCommentType(span, false);
            if (tokenType == TokenType::VALUE)
            {
                auto result = findEndOfValue(span, false);
                result.tokenType = TokenType::INVALID_TOKEN;
                return result;
            }

            if (tokenType == TokenType::END_OF_BUFFER)
                return Result(TokenType::COMMENT, span.end(), true);

            span.begin() += 2;

            if (tokenType == TokenType::COMMENT)
                return findEndOfLineComment(span, false);
            else
                return findEndOfBlockComment(span, false, false);
        }

        Result nextStringToken(TokenSpan span)
        {
            auto tokenType = determineStringType(span, 0, false);
            if (!tokenType.second)
                return Result(tokenType.first, span.end(), true);
            if (tokenType.first == TokenType::BLOCK_STRING)
            {
                span.begin() += 3;
                return findEndOfBlockString(span, 0, false, false);
            }
            if (tokenType.first != TokenType::STRING)
                return Result(tokenType.first, span.end(), false);

            span.begin() += 1;
            return findEndOfString(span, false, false);
        }

        Result findEndOfBlockComment(TokenSpan span, bool precededByStar,
                                     bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '/' && precededByStar)
                    return Result(TokenType::BLOCK_COMMENT, ++it);
                precededByStar = *it == '*';
            }
            if (isEndOfFile)
                return Result(TokenType::INVALID_TOKEN, span.end());
            return Result(TokenType::BLOCK_COMMENT, span.end(), true);
        }

        Result findEndOfBlockString(TokenSpan span, size_t precedingQuotes,
                                    bool escapeFirst, bool isEndOfFile)
        {
            assert(!escapeFirst || precedingQuotes == 0);

            auto quotes = precedingQuotes;
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (escapeFirst)
                    escapeFirst = false;
                else if (*it == '"')
                    ++quotes;
                else if (quotes < 3)
                    quotes = 0;
                else
                    return Result(TokenType::BLOCK_STRING, it);
            }
            if (!isEndOfFile)
                return Result(TokenType::BLOCK_STRING, span.end(), true);
            else if (quotes >= 3)
                return Result(TokenType::BLOCK_STRING, span.end());
            else
                return Result(TokenType::INVALID_TOKEN, span.end());
        }

        Result findEndOfLineComment(TokenSpan span, bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '\r' || *it == '\n')
                    return Result(TokenType::COMMENT, it);
            }
            return Result(TokenType::COMMENT, span.end(), !isEndOfFile);
        }

        Result findEndOfNewline(TokenSpan span, bool precededByCr,
                                bool isEndOfFile)
        {
            assert(precededByCr
                   || (!isEmpty(span)
                       && (span.front() == '\n' || span.front() == '\r')));

            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '\n')
                    return Result(TokenType::NEWLINE, ++it);
                else if (precededByCr)
                    return Result(TokenType::NEWLINE, it);
                precededByCr = true;
            }
            return Result(TokenType::NEWLINE, span.end(), !isEndOfFile);
        }

        Result findEndOfString(TokenSpan span, bool escapeFirst,
                               bool isEndOfFile)
        {
            bool valid = true;
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it < 0x20)
                {
                    if (*it == '\n')
                        return Result(TokenType::INVALID_TOKEN, it);
                    valid = false;
                }
                else if (escapeFirst)
                {
                    escapeFirst = false;
                }
                else if (*it == '"')
                {
                    return Result(valid ? TokenType::STRING
                                        : TokenType::INVALID_TOKEN,
                                  ++it);
                }
                else if (*it == '\\')
                {
                    escapeFirst = true;
                }
            }
            if (isEndOfFile)
                return Result(TokenType::INVALID_TOKEN, span.end());
            else
                return Result(TokenType::STRING, span.end(), true);
        }

        Result findEndOfValue(TokenSpan span, bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                switch (*it)
                {
                case '\t':
                case '\n':
                case '\r':
                case ' ':
                case '"':
                case ',':
                case ':':
                case '[':
                case ']':
                case '{':
                case '}':
                    return Result(TokenType::VALUE, it);
                default:
                    break;
                }
            }
            return Result(TokenType::VALUE, span.end(), !isEndOfFile);
        }

        Result findEndOfWhitespace(TokenSpan span, bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it != ' ' && *it != '\t')
                    return Result(TokenType::WHITESPACE, it);
            }
            return Result(TokenType::WHITESPACE, span.end(),
                          !isEndOfFile);
        }

        bool endsWithEscape(TokenSpan span)
        {
            auto revSpan = makeReverseSpan(span);
            auto end = std::find_if(revSpan.begin(), revSpan.end(),
                                    [](char c){return c != '\\';});
            return std::distance(revSpan.begin(), end) % 2 == 1;
        }

        bool endsWithStar(TokenSpan span)
        {
            return span.size() > 2 && span.back() == '*';
        }

        size_t countQuotesAtEnd(TokenSpan span)
        {
            auto revSpan = makeReverseSpan(span);
            auto qend = std::find_if(revSpan.begin(), revSpan.end(),
                                     [](char c){return c != '"';});
            auto quotes = std::distance(revSpan.begin(), qend);
            if  (quotes == 0)
                return 0;
            auto eend = std::find_if(qend, revSpan.end(),
                                     [](char c){return c != '\\';});
            bool escaped = std::distance(qend, eend) % 2 == 1;
            return size_t(quotes) - (escaped ? 1 : 0);
        }

        std::pair<TokenType, bool> determineStringType(TokenSpan span,
                                                       size_t precedingQuotes,
                                                       bool isEndOfFile)
        {
            assert((!isEmpty(span) && span.front() == '"')
                   || precedingQuotes != 0);

            size_t quotes = precedingQuotes;
            auto it = span.begin();
            while (it != span.end() && *it == '"' && quotes < 3)
            {
                ++it;
                ++quotes;
            }

            if (quotes == 3)
                return std::make_pair(TokenType::BLOCK_STRING, true);
            else if (it != span.end())
                // Ignores on purpose the case where quotes is 0. This
                // function won't be called unless *beg is '"'
                // or precedingQuotes > 0.
                return std::make_pair(TokenType::STRING, true);
            else if (!isEndOfFile)
                return std::make_pair(TokenType::STRING, false);
            else if (quotes == 2)
                return std::make_pair(TokenType::STRING, true);
            else
                return std::make_pair(TokenType::INVALID_TOKEN, true);
        }

        TokenType determineCommentType(TokenSpan span,
                                       bool precededBySlash)
        {
            assert(precededBySlash
                   || (!isEmpty(span) && span.front() == '/'));
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '*' && precededBySlash)
                    return TokenType::BLOCK_COMMENT;
                else if (*it != '/')
                    return TokenType::VALUE;
                else if (precededBySlash)
                    return TokenType::COMMENT;
                else
                    precededBySlash = true;
            }
            return TokenType::END_OF_BUFFER;
        }
    }
}
