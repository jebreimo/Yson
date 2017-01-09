//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-03-06.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Tokenizer.hpp"

#include <functional>
#include "Detail/Span.hpp"
#include "Detail/SpanFunctions.hpp"

namespace Yson
{
    using namespace std::placeholders;

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
}
