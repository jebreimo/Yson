//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizer.hpp"

#include <cassert>
#include <functional>

namespace Yson
{
    using namespace std::placeholders;

    template <typename BiIt>
    JsonTokenizer::TokenType determineCommentType(
            BiIt beg, BiIt end, size_t precededBySlash = false);

    template<typename BiIt>
    JsonTokenizer::TokenType determineStringType(BiIt beg, BiIt end,
                                                 size_t precedingQuotes = 0,
                                                 bool endOfFile = false);

    JsonTokenizer::JsonTokenizer()
        : m_BufferStart(NULL),
          m_BufferEnd(NULL),
          m_TokenStart(NULL),
          m_TokenEnd(NULL),
          m_TokenType(INVALID_TOKEN)
    {}

    bool JsonTokenizer::hasNext() const
    {
        return !endOfFile() ||
               !m_InternalBuffer.empty();
    }

    void JsonTokenizer::next()
    {
        m_TokenStart = m_TokenEnd;
        if (!m_InternalBuffer.empty() && m_TokenType == END_OF_BUFFER)
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
        auto range = rawToken();
        return std::string(range.first, range.second);
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
                    &m_InternalBuffer[0],
                    &m_InternalBuffer[0] + m_InternalBuffer.size());
        }
    }

    void JsonTokenizer::reset()
    {
        m_TokenStart = m_TokenEnd = m_BufferStart;
        m_TokenType = INVALID_TOKEN;
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
        case '0':
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
        if (tokenType == END_OF_BUFFER)
        {
            tokenType = determineCommentType(m_TokenStart, m_BufferEnd, true);
            if (tokenType == COMMENT || tokenType == BLOCK_COMMENT)
                ++m_TokenEnd;
        }

        if (tokenType == COMMENT)
            return findEndOfLineComment();
        else if (tokenType == BLOCK_COMMENT)
            return findEndOfBlockComment(endsWithStar());
        else
            return findEndOfValue();
    }

    JsonTokenizer::TokenType JsonTokenizer::completeStringToken()
    {
        JsonTokenizer::TokenType tokenType = determineStringType(
                m_InternalBuffer.begin(), m_InternalBuffer.end());
        if (tokenType == END_OF_BUFFER)
        {
            tokenType = determineStringType(m_TokenStart, m_BufferEnd,
                                            m_InternalBuffer.size(),
                                            endOfFile());
            if (tokenType == BLOCK_STRING)
                m_TokenEnd = m_TokenStart + (3 - m_InternalBuffer.size());
            else if (tokenType == STRING && endOfFile())
                return STRING;
        }

        if (tokenType == STRING)
        {
            return findEndOfString(endsWithEscape());
        }
        else if (tokenType != BLOCK_STRING)
        {
            return tokenType;
        }
        else if (m_TokenStart == m_TokenEnd)
        {
            size_t quotes = countQuotesAtEnd();
            if (quotes == m_InternalBuffer.size())
                quotes -= 3;
            return findEndOfBlockString(quotes,
                                        quotes == 0 && endsWithEscape());
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
            return END_OF_BUFFER;
        switch (*m_TokenStart)
        {
        case ' ':
        case '\t':
            return findEndOfWhitespace();
        case '\r':
        case '\n':
            return nextNewlineToken();
        case '[': SINGLECHAR_TOKEN(START_ARRAY);
        case ']': SINGLECHAR_TOKEN(END_ARRAY);
        case '{': SINGLECHAR_TOKEN(START_OBJECT);
        case '}': SINGLECHAR_TOKEN(END_OBJECT);
        case ':': SINGLECHAR_TOKEN(COLON);
        case ',': SINGLECHAR_TOKEN(COMMA);
        case '"': return nextStringToken();
        case '/': return nextCommentToken();
        default:
            return nextValueToken();
        }
    }

    JsonTokenizer::TokenType JsonTokenizer::nextCommentToken()
    {
        assert(m_TokenStart != m_BufferEnd);
        assert(*m_TokenStart == '/');

        TokenType tokenType = determineCommentType(
                m_TokenStart, m_BufferEnd, false);
        if (tokenType == VALUE)
            return nextValueToken();
        else if (tokenType == END_OF_BUFFER)
          ++m_TokenEnd;
        else
          m_TokenEnd += 2;

        if (tokenType == COMMENT)
            tokenType = findEndOfLineComment();
        else
            tokenType = findEndOfBlockComment(false);

        if (tokenType == END_OF_BUFFER)
            appendTokenToInternalBuffer();
        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextNewlineToken()
    {
        TokenType tokenType = findEndOfNewline(false);
        if (tokenType == END_OF_BUFFER)
            appendTokenToInternalBuffer();
        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextStringToken()
    {
        JsonTokenizer::TokenType tokenType = determineStringType(
                m_TokenStart, m_BufferEnd, 0, endOfFile());
        if (tokenType == BLOCK_STRING)
        {
            m_TokenEnd = m_TokenStart + 3;
            tokenType = findEndOfBlockString(0, false);
        }
        else if (tokenType == STRING)
        {
            m_TokenEnd = m_TokenStart + 1;
            tokenType = findEndOfString(false);
        }
        else
        {
            m_TokenEnd = m_BufferEnd;
        }

        if (tokenType == END_OF_BUFFER)
            appendTokenToInternalBuffer();

        return tokenType;
    }

    JsonTokenizer::TokenType JsonTokenizer::nextValueToken()
    {
        m_TokenEnd = m_TokenStart + 1;
        TokenType type = findEndOfValue();
        if (type == END_OF_BUFFER)
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
                return BLOCK_STRING;
            ++m_TokenEnd;
        }
        if (endOfFile())
            return quotes >= 3 ? BLOCK_STRING : INVALID_TOKEN;
        else
            return END_OF_BUFFER;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfBlockComment(
            bool precededByStar)
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd == '/' && precededByStar)
            {
                ++m_TokenEnd;
                return BLOCK_COMMENT;
            }
            precededByStar = *m_TokenEnd == '*';
            ++m_TokenEnd;
        }
        return endOfFile() ? INVALID_TOKEN : END_OF_BUFFER;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfLineComment()
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd == '\r' || *m_TokenEnd == '\n')
                return COMMENT;
            ++m_TokenEnd;
        }
        return endOfFile() ? COMMENT : END_OF_BUFFER;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfNewline(
            bool precededByCr)
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
                return NEWLINE;
            }
            else if (precededByCr)
            {
                return NEWLINE;
            }

            precededByCr = true;
            ++m_TokenEnd;
        }
        return endOfFile() ? NEWLINE : END_OF_BUFFER;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfString(bool escapeFirst)
    {
        bool valid = true;
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd < 0x20)
            {
                if (*m_TokenEnd == '\n')
                    return INVALID_TOKEN;
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
                return valid ? STRING : INVALID_TOKEN;
            }
            else if (*m_TokenEnd == '\\')
            {
                escapeFirst = true;
            }

            ++m_TokenEnd;
        }
        return endOfFile() ? INVALID_TOKEN : END_OF_BUFFER;
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
                return VALUE;
            }
            ++m_TokenEnd;
        }
        return endOfFile() ? VALUE : END_OF_BUFFER;
    }

    JsonTokenizer::TokenType JsonTokenizer::findEndOfWhitespace()
    {
        while (m_TokenEnd != m_BufferEnd)
        {
            if (*m_TokenEnd != ' ' && *m_TokenEnd != '\t')
                return WHITESPACE;
            ++m_TokenEnd;
        }
        return WHITESPACE;
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
        auto quotes = size_t(std::distance(m_InternalBuffer.rbegin(), qend));
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
            return JsonTokenizer::BLOCK_STRING;
        else if (beg != end)
            // Ignores on purpose the case where quotes is 0. This function
            // won't be called unless *beg is '"' or precedingQuotes > 0.
            return JsonTokenizer::STRING;
        else if (!endOfFile)
            return JsonTokenizer::END_OF_BUFFER;
        else if (quotes == 2)
            return JsonTokenizer::STRING;
        else
            return JsonTokenizer::INVALID_TOKEN;
    }

    template <typename BiIt>
    JsonTokenizer::TokenType determineCommentType(BiIt beg, BiIt end,
                                                  size_t precededBySlash)
    {
        while (beg != end)
        {
            if (*beg == '*' && precededBySlash)
                return JsonTokenizer::BLOCK_COMMENT;
            else if (*beg != '/')
                return JsonTokenizer::VALUE;
            else if (precededBySlash)
                return JsonTokenizer::COMMENT;
            else
                precededBySlash = true;
            ++beg;
        }
        return JsonTokenizer::END_OF_BUFFER;
    }
}
