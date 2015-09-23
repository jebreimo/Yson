//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizer.hpp"

//#include <algorithm>
#include <cassert>
#include <functional>

namespace Yson
{
    using namespace std::placeholders;

    template <typename BiIt>
    JsonTokenizer::TokenType determineCommentType(BiIt beg, BiIt end,
                                                  size_t precededBySlash = false);
    template <typename BiIt>
    JsonTokenizer::TokenType determineStringType(BiIt beg, BiIt end,
                                                 size_t precedingQuotes = 0,
                                                 bool endOfFile = false);

    JsonTokenizer::JsonTokenizer()
        : m_BufferStart(NULL),
          m_BufferEnd(NULL),
          m_TokenStart(NULL),
          m_TokenEnd(NULL),
          m_TokenType(InvalidToken)
    {
    }

    bool JsonTokenizer::hasNext() const
    {
        return !endOfFile() ||
               !m_InternalBuffer.empty();
    }

    void JsonTokenizer::next()
    {
        m_TokenStart = m_TokenEnd;
        if (!m_InternalBuffer.empty() && m_TokenType == EndOfBuffer)
        {
            m_TokenType = completeCurrentToken();
        }
        else
        {
            m_InternalBuffer.clear();
            m_TokenType = nextToken();
        }
    }

    JsonTokenizer::TokenType JsonTokenizer::tokenType() const
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
        return (size_t)(m_BufferEnd - m_BufferStart);
    }

    std::string JsonTokenizer::token() const
    {
        const char *start, *end;
        getToken(start, end);
        return std::string(start, end);
    }

    void JsonTokenizer::getToken(const char*& start, const char*& end) const
    {
        if (m_InternalBuffer.empty())
        {
            start = m_TokenStart;
            end = m_TokenEnd;
        }
        else
        {
            start = &m_InternalBuffer[0];
            end = &m_InternalBuffer[0] + m_InternalBuffer.size();
        }
    }

    void JsonTokenizer::reset()
    {
        m_TokenStart = m_TokenEnd = m_BufferStart;
        m_TokenType = InvalidToken;
        m_InternalBuffer.clear();
    }

    JsonTokenizer::TokenType JsonTokenizer::completeCurrentToken()
    {
        TokenType tokenType;
        switch (m_InternalBuffer.front())
        {
        case '"':
            tokenType = completeStringToken();
            break;
        case '\r':
            tokenType = findEndOfNewline(true);
            break;
        case '/':
            tokenType = completeCommentToken();
            break;
        default:
            tokenType = findEndOfValue();
            break;
        }
        appendTokenToInternalBuffer();
        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::completeCommentToken()
    {
        JsonTokenizer::TokenType tokenType = determineCommentType(
                m_InternalBuffer.begin(), m_InternalBuffer.end());
        if (tokenType == EndOfBuffer)
        {
            tokenType = determineCommentType(m_TokenStart, m_BufferEnd, true);
            if (tokenType == Comment || tokenType == BlockComment)
                ++m_TokenEnd;
        }

        if (tokenType == Comment)
            return findEndOfLineComment();
        else if (tokenType == BlockComment)
            return findEndOfBlockComment(endsWithStar());
        else
            return findEndOfValue();
    }

    JsonTokenizer::TokenType JsonTokenizer::completeStringToken()
    {
        JsonTokenizer::TokenType tokenType = determineStringType(
                m_InternalBuffer.begin(), m_InternalBuffer.end());
        if (tokenType == EndOfBuffer)
        {
            tokenType = determineStringType(m_TokenStart, m_BufferEnd,
                                            m_InternalBuffer.size(),
                                            endOfFile());
            if (tokenType == BlockString)
                m_TokenEnd = m_TokenStart + (3 - m_InternalBuffer.size());
        else if (tokenType == String && endOfFile())
            return String;
        }

        if (tokenType == String)
        {
            return findEndOfString(endsWithEscape());
        }
        else if (tokenType != BlockString)
        {
            return tokenType;
        }
        else if (m_TokenStart == m_TokenEnd)
        {
            size_t quotes = countQuotesAtEnd();
            if (quotes == m_InternalBuffer.size())
                quotes -= 3;
            return findEndOfBlockString(quotes, quotes == 0 && endsWithEscape());
        }
        else
        {
            return findEndOfBlockString(0, false);
        }
    }

    #define SINGLECHAR_TOKEN(type) \
        m_TokenEnd = m_TokenStart + 1; \
        return type

    JsonTokenizer::TokenType JsonTokenizer::nextToken()
    {
        if (m_TokenStart == m_BufferEnd)
            return EndOfBuffer;
        switch (*m_TokenStart)
        {
        case ' ':
        case '\t':
            return findEndOfWhitespace();
        case '\r':
        case '\n':
            return nextNewlineToken();
        case '[': SINGLECHAR_TOKEN(ArrayStart);
        case ']': SINGLECHAR_TOKEN(ArrayEnd);
        case '{': SINGLECHAR_TOKEN(ObjectStart);
        case '}': SINGLECHAR_TOKEN(ObjectEnd);
        case ':': SINGLECHAR_TOKEN(Colon);
        case ',': SINGLECHAR_TOKEN(Comma);
        case '"': return nextStringToken();
        case '/': return nextCommentToken();
        default:
            return nextValueToken();
        }
        return InvalidToken;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextCommentToken()
    {
        assert(m_TokenStart != m_BufferEnd);
        assert(*m_TokenStart == '/');

        TokenType tokenType = determineCommentType(m_TokenStart, m_BufferEnd, false);
        if (tokenType == Value)
            return nextValueToken();
        else if (tokenType == EndOfBuffer)
          ++m_TokenEnd;
        else
          m_TokenEnd += 2;

        if (tokenType == Comment)
            tokenType = findEndOfLineComment();
        else
            tokenType = findEndOfBlockComment(false);

        if (tokenType == EndOfBuffer)
            appendTokenToInternalBuffer();
        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextNewlineToken()
    {
        TokenType tokenType = findEndOfNewline(false);
        if (tokenType == EndOfBuffer)
            appendTokenToInternalBuffer();
        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextStringToken()
    {
        JsonTokenizer::TokenType tokenType = determineStringType(
                m_TokenStart, m_BufferEnd, 0, endOfFile());
        if (tokenType == BlockString)
        {
            m_TokenEnd = m_TokenStart + 3;
            tokenType = findEndOfBlockString(0, false);
        }
        else if (tokenType == String)
        {
            m_TokenEnd = m_TokenStart + 1;
            tokenType = findEndOfString(false);
        }
        else
        {
            m_TokenEnd = m_BufferEnd;
        }

        if (tokenType == EndOfBuffer)
            appendTokenToInternalBuffer();

        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextValueToken()
    {
        m_TokenEnd = m_TokenStart + 1;
        TokenType type = findEndOfValue();
        if (type == EndOfBuffer)
            appendTokenToInternalBuffer();
        return type;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfBlockString(
            size_t precedingQuotes,
            bool escapeFirst)
    {
        assert(!escapeFirst || precedingQuotes == 0);

        size_t quotes = precedingQuotes;
        while (m_TokenEnd != m_BufferEnd)
        {
            if (escapeFirst)
                escapeFirst = false;
            else if (*m_TokenEnd == '"')
                ++quotes;
            else if (quotes < 3)
                quotes = 0;
            else
                return BlockString;
            ++m_TokenEnd;
        }
        if (endOfFile())
            return quotes >= 3 ? BlockString : InvalidToken;
        else
            return EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfBlockComment(bool precededByStar)
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd == '/' && precededByStar)
            {
                ++m_TokenEnd;
                return BlockComment;
            }
            precededByStar = *m_TokenEnd == '*';
            ++m_TokenEnd;
        }
        return endOfFile() ? InvalidToken : EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfLineComment()
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd == '\r' || *m_TokenEnd == '\n')
                return Comment;
            ++m_TokenEnd;
        }
        return endOfFile() ? Comment : EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfNewline(bool precededByCr)
    {
        assert(precededByCr ||
               m_TokenEnd != m_BufferEnd &&
                (*m_TokenEnd == '\n' ||
                 *m_TokenEnd == '\r'));

        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd == '\n')
            {
                ++m_TokenEnd;
                return Newline;
            }
            else if (precededByCr)
            {
                return Newline;
            }

            precededByCr = true;
            ++m_TokenEnd;
        }
        return endOfFile() ? Newline : EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfString(bool escapeFirst)
    {
        bool valid = true;
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd < 0x20)
            {
                if (*m_TokenEnd == '\n')
                    return InvalidToken;
                else
                    valid = false;
            }
            else if (escapeFirst)
            {
                escapeFirst = false;
            }
            else if (*m_TokenEnd == '"')
            {
                ++m_TokenEnd;
                return valid ? String : InvalidToken;
            }
            else if (*m_TokenEnd == '\\')
            {
                escapeFirst = true;
            }

            ++m_TokenEnd;
        }
        return endOfFile() ? InvalidToken : EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfValue()
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            switch (*m_TokenEnd)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
            case ',':
            case ']':
            case '}':
                return Value;
            }
            ++m_TokenEnd;
        }
        return endOfFile() ? Value : EndOfBuffer;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfWhitespace()
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd != ' ' && *m_TokenEnd != '\t')
                return Whitespace;
            ++m_TokenEnd;
        }
        return Whitespace;
    }

    bool JsonTokenizer::endsWithEscape() const
    {
        auto end = std::find_if(m_InternalBuffer.rbegin(),
                                m_InternalBuffer.rend(),
                                [](char c){return c != '\\';});
        return std::distance(m_InternalBuffer.rbegin(), end) % 2 == 1;
    }

    bool JsonTokenizer::endsWithStar() const
    {
        return m_InternalBuffer.size() > 2 && m_InternalBuffer.back() == '*';
    }

    size_t JsonTokenizer::countQuotesAtEnd() const
    {
        auto qend = std::find_if(m_InternalBuffer.rbegin(),
                                 m_InternalBuffer.rend(),
                                 [](char c){return c != '"';});
        size_t quotes = (size_t)std::distance(m_InternalBuffer.rbegin(), qend);
        if  (quotes == 0)
            return quotes;
        auto eend = std::find_if(qend,
                                 m_InternalBuffer.rend(),
                                 [](char c){return c != '\\';});
        bool escaped = std::distance(qend, eend) % 2 == 1;
        return quotes - (escaped ? 1 : 0);
    }

    bool JsonTokenizer::endOfFile() const
    {
        return m_BufferStart == m_BufferEnd;
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

    template <typename BiIt>
    JsonTokenizer::TokenType determineStringType(BiIt beg, BiIt end,
                                                 size_t precedingQuotes,
                                                 bool endOfFile)
    {
        assert((beg != end && *beg == '"') || precedingQuotes != 0);

        size_t quotes = precedingQuotes;
        while (beg != end && *beg == '"' && quotes < 3)
        {
            ++beg;
            ++quotes;
        }

        if (quotes >= 3)
            return JsonTokenizer::BlockString;
        else if (beg != end)
            // Ignores on purpose the case where quotes is 0. This function won't
            // be called unless *beg is '"' or precedingQuotes > 0.
            return JsonTokenizer::String;
        else if (!endOfFile)
            return JsonTokenizer::EndOfBuffer;
        else if (quotes == 2)
            return JsonTokenizer::String;
        else
            return JsonTokenizer::InvalidToken;
    }

    template <typename BiIt>
    JsonTokenizer::TokenType determineCommentType(BiIt beg, BiIt end,
                                                  size_t precededBySlash)
    {
        while (beg != end)
        {
            if (*beg == '*' && precededBySlash)
                return JsonTokenizer::BlockComment;
            else if (*beg != '/')
                return JsonTokenizer::Value;
            else if (precededBySlash)
                return JsonTokenizer::Comment;
            else
                precededBySlash = true;
            ++beg;
        }
        return JsonTokenizer::EndOfBuffer;
    }
}
