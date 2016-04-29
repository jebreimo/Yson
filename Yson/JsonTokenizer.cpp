//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizer.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include "Span.hpp"

namespace Yson
{
    using namespace std::placeholders;

    typedef Span<const char*> TokenSpan;

    namespace {
        struct Result
        {
            Result(JsonTokenType_t tokenType, const char* endOfToken)
                : tokenType(tokenType),
                  endOfToken(endOfToken),
                  isIncomplete(false)
            {}

            Result(JsonTokenType_t tokenType,
                   const char* endOfToken,
                   bool isIncomplete)
                : tokenType(tokenType),
                  endOfToken(endOfToken),
                  isIncomplete(isIncomplete)
            {}

            JsonTokenType_t tokenType;
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

        JsonTokenType_t determineCommentType(TokenSpan span,
                                             bool precededBySlash);

        std::pair<JsonTokenType_t, bool> determineStringType(
                TokenSpan span,
                size_t precedingQuotes,
                bool isEndOfFile);
    }

    JsonTokenizer::JsonTokenizer()
        : m_BufferStart(nullptr),
          m_BufferEnd(nullptr),
          m_TokenStart(nullptr),
          m_TokenEnd(nullptr),
          m_TokenType(JsonTokenType::INVALID_TOKEN)
    {}

    JsonTokenizer::~JsonTokenizer()
    {}

    bool JsonTokenizer::hasNext() const
    {
        return !isEndOfFile() || !m_InternalBuffer.empty();
    }

    void JsonTokenizer::next()
    {
        m_TokenStart = m_TokenEnd;
        if (m_InternalBuffer.empty()
            || m_TokenType != JsonTokenType::END_OF_BUFFER)
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
                m_TokenType = JsonTokenType::END_OF_BUFFER;
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
            m_TokenType = result.isIncomplete ? JsonTokenType::END_OF_BUFFER
                                              : result.tokenType;
            m_TokenEnd = result.endOfToken;
            appendTokenToInternalBuffer();
        }
    }

    std::pair<JsonTokenType_t, bool> JsonTokenizer::peek() const
    {
        if (!m_InternalBuffer.empty()
            && m_TokenType == JsonTokenType::END_OF_BUFFER)
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

    JsonTokenType_t JsonTokenizer::tokenType() const
    {
        return m_TokenType;
    }

    const char* JsonTokenizer::buffer() const
    {
        return m_BufferStart;
    }

    void JsonTokenizer::setBuffer(const char* buffer, size_t size)
    {
        m_BufferStart = m_TokenStart = m_TokenEnd = buffer;
        m_BufferEnd = buffer + size;
    }

    size_t JsonTokenizer::bufferSize() const
    {
        return size_t(m_BufferEnd - m_BufferStart);
    }

    size_t JsonTokenizer::currentPosition() const
    {
        return size_t(m_TokenEnd - m_BufferStart);
    }

    std::string JsonTokenizer::token() const
    {
        auto span = rawToken();
        return std::string(span.first, span.second);
    }

    std::pair<const char*, const char*> JsonTokenizer::rawToken() const
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

    void JsonTokenizer::reset()
    {
        m_TokenStart = m_TokenEnd = m_BufferStart;
        m_TokenType = JsonTokenType::INVALID_TOKEN;
        m_InternalBuffer.clear();
    }

    void JsonTokenizer::appendTokenToInternalBuffer()
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

    bool JsonTokenizer::isEndOfFile() const
    {
        return m_BufferStart == m_BufferEnd;
    }

    namespace {
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
            case '0':
            default:
                return findEndOfValue(secondPart, isEndOfFile);
            }
        }

        Result completeCommentToken(TokenSpan firstPart,
                                    TokenSpan secondPart,
                                    bool isEndOfFile)
        {
            auto tokenType = determineCommentType(firstPart, false);
            if (tokenType == JsonTokenType::END_OF_BUFFER)
            {
                if (isEndOfFile)
                {
                    return Result(JsonTokenType::INVALID_TOKEN,
                                  secondPart.end());
                }
                tokenType = determineCommentType(secondPart, true);
                if (tokenType == JsonTokenType::COMMENT
                    || tokenType == JsonTokenType::BLOCK_COMMENT)
                {
                    ++secondPart.begin();
                }
            }

            if (tokenType == JsonTokenType::COMMENT)
                return findEndOfLineComment(secondPart, isEndOfFile);
            if (tokenType == JsonTokenType::BLOCK_COMMENT)
                return findEndOfBlockComment(secondPart,
                                             endsWithStar(firstPart),
                                             isEndOfFile);

            auto result = findEndOfValue(secondPart, isEndOfFile);
            result.tokenType = JsonTokenType::INVALID_TOKEN;
            return result;
        }

        Result completeStringToken(TokenSpan firstPart,
                                   TokenSpan secondPart, bool isEndOfFile)
        {
            auto tokenType = determineStringType(firstPart, 0, false);
            if (!tokenType.second)
            {
                tokenType = determineStringType(secondPart,
                                                firstPart.size(),
                                                isEndOfFile);
                if (tokenType.first == JsonTokenType::STRING && isEndOfFile)
                    return Result(JsonTokenType::STRING, secondPart.end());
                if (tokenType.first == JsonTokenType::BLOCK_STRING)
                    secondPart.begin() += 3 - firstPart.size();
            }

            if (tokenType.first == JsonTokenType::STRING)
            {
                return findEndOfString(secondPart, endsWithEscape(firstPart),
                                       isEndOfFile);
            }
            else if (tokenType.first != JsonTokenType::BLOCK_STRING)
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
                return Result(JsonTokenType::END_OF_BUFFER, span.end());
            switch (span.front())
            {
            case ' ':
            case '\t':
                return findEndOfWhitespace(span, false);
            case '\r':
            case '\n':
                return findEndOfNewline(span, false, false);
            case '[':
                return Result(JsonTokenType::START_ARRAY, span.begin() + 1);
            case ']':
                return Result(JsonTokenType::END_ARRAY, span.begin() + 1);
            case '{':
                return Result(JsonTokenType::START_OBJECT, span.begin() + 1);
            case '}':
                return Result(JsonTokenType::END_OBJECT, span.begin() + 1);
            case ':':
                return Result(JsonTokenType::COLON, span.begin() + 1);
            case ',':
                return Result(JsonTokenType::COMMA, span.begin() + 1);
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
            if (tokenType == JsonTokenType::VALUE)
            {
                auto result = findEndOfValue(span, false);
                result.tokenType = JsonTokenType::INVALID_TOKEN;
                return result;
            }

            if (tokenType == JsonTokenType::END_OF_BUFFER)
                return Result(JsonTokenType::COMMENT, span.end(), true);

            span.begin() += 2;

            if (tokenType == JsonTokenType::COMMENT)
                return findEndOfLineComment(span, false);
            else
                return findEndOfBlockComment(span, false, false);
        }

        Result nextStringToken(TokenSpan span)
        {
            auto tokenType = determineStringType(span, 0, false);
            if (!tokenType.second)
                return Result(tokenType.first, span.end(), true);
            if (tokenType.first == JsonTokenType::BLOCK_STRING)
            {
                span.begin() += 3;
                return findEndOfBlockString(span, 0, false, false);
            }
            if (tokenType.first != JsonTokenType::STRING)
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
                    return Result(JsonTokenType::BLOCK_COMMENT, ++it);
                precededByStar = *it == '*';
            }
            if (isEndOfFile)
                return Result(JsonTokenType::INVALID_TOKEN, span.end());
            return Result(JsonTokenType::BLOCK_COMMENT, span.end(), true);
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
                    return Result(JsonTokenType::BLOCK_STRING, it);
            }
            if (!isEndOfFile)
                return Result(JsonTokenType::BLOCK_STRING, span.end(), true);
            else if (quotes >= 3)
                return Result(JsonTokenType::BLOCK_STRING, span.end());
            else
                return Result(JsonTokenType::INVALID_TOKEN, span.end());
        }

        Result findEndOfLineComment(TokenSpan span, bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '\r' || *it == '\n')
                    return Result(JsonTokenType::COMMENT, it);
            }
            return Result(JsonTokenType::COMMENT, span.end(), !isEndOfFile);
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
                    return Result(JsonTokenType::NEWLINE, ++it);
                else if (precededByCr)
                    return Result(JsonTokenType::NEWLINE, it);
                precededByCr = true;
            }
            return Result(JsonTokenType::NEWLINE, span.end(), !isEndOfFile);
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
                        return Result(JsonTokenType::INVALID_TOKEN, it);
                    valid = false;
                }
                else if (escapeFirst)
                {
                    escapeFirst = false;
                }
                else if (*it == '"')
                {
                    return Result(valid ? JsonTokenType::STRING
                                        : JsonTokenType::INVALID_TOKEN,
                                  ++it);
                }
                else if (*it == '\\')
                {
                    escapeFirst = true;
                }
            }
            if (isEndOfFile)
                return Result(JsonTokenType::INVALID_TOKEN, span.end());
            else
                return Result(JsonTokenType::STRING, span.end(), true);
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
                    return Result(JsonTokenType::VALUE, it);
                default:
                    break;
                }
            }
            return Result(JsonTokenType::VALUE, span.end(), !isEndOfFile);
        }

        Result findEndOfWhitespace(TokenSpan span, bool isEndOfFile)
        {
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it != ' ' && *it != '\t')
                    return Result(JsonTokenType::WHITESPACE, it);
            }
            return Result(JsonTokenType::WHITESPACE, span.end(),
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

        std::pair<JsonTokenType_t, bool> determineStringType(
                TokenSpan span,
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
                return std::make_pair(JsonTokenType::BLOCK_STRING, true);
            else if (it != span.end())
                // Ignores on purpose the case where quotes is 0. This
                // function won't be called unless *beg is '"'
                // or precedingQuotes > 0.
                return std::make_pair(JsonTokenType::STRING, true);
            else if (!isEndOfFile)
                return std::make_pair(JsonTokenType::STRING, false);
            else if (quotes == 2)
                return std::make_pair(JsonTokenType::STRING, true);
            else
                return std::make_pair(JsonTokenType::INVALID_TOKEN, true);
        }

        JsonTokenType_t determineCommentType(TokenSpan span,
                                             bool precededBySlash)
        {
            assert(precededBySlash
                   || (!isEmpty(span) && span.front() == '/'));
            for (auto it = span.begin(); it != span.end(); ++it)
            {
                if (*it == '*' && precededBySlash)
                    return JsonTokenType::BLOCK_COMMENT;
                else if (*it != '/')
                    return JsonTokenType::VALUE;
                else if (precededBySlash)
                    return JsonTokenType::COMMENT;
                else
                    precededBySlash = true;
            }
            return JsonTokenType::END_OF_BUFFER;
        }
    }
}
