//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizer.hpp"

#include <cassert>
#include <tuple>
#include "Yson/Common/DefaultBufferSize.hpp"
#include "Yson/Common/ThrowYsonException.hpp"
#include "JsonTokenizerUtilities.hpp"
#include "TextBufferReader.hpp"
#include "TextFileReader.hpp"

namespace Yson
{
    JsonTokenizer::JsonTokenizer(std::istream& stream,
                                 const char* buffer,
                                 size_t bufferSize)
        : m_TextReader(new TextStreamReader(stream, buffer, bufferSize)),
          m_ChunkSize(getDefaultBufferSize())
    {}

    JsonTokenizer::JsonTokenizer(const std::string& fileName)
        : m_TextReader(new TextFileReader(fileName)),
          m_FileName(fileName),
          m_ChunkSize(getDefaultBufferSize())
    {}

    JsonTokenizer::JsonTokenizer(const char* buffer, size_t bufferSize)
        : m_TextReader(new TextBufferReader(buffer, bufferSize)),
          m_ChunkSize(getDefaultBufferSize())
    {}

    JsonTokenizer::~JsonTokenizer() = default;

    JsonTokenizer::JsonTokenizer(JsonTokenizer&&) noexcept = default;

    bool JsonTokenizer::next()
    {
        while (internalNext())
        {
            switch (m_TokenType)
            {
            case JsonTokenType::INVALID_TOKEN:
                m_ColumnNumber += m_TokenEnd - m_TokenStart;
                return false;
            case JsonTokenType::START_ARRAY:
            case JsonTokenType::END_ARRAY:
            case JsonTokenType::START_OBJECT:
            case JsonTokenType::END_OBJECT:
            case JsonTokenType::COLON:
            case JsonTokenType::COMMA:
            case JsonTokenType::VALUE:
                m_ColumnNumber += m_TokenEnd - m_TokenStart;
                return true;
            case JsonTokenType::STRING:
                m_ColumnNumber += m_TokenEnd-- - m_TokenStart++;
                return true;
            case JsonTokenType::INTERNAL_MULTILINE_STRING:
                addLinesAndColumns(m_LineNumber, m_ColumnNumber,
                                   countLinesAndColumns(token()));
                removeLineContinuations();
                m_TokenType = JsonTokenType::STRING;
                return true;
            case JsonTokenType::INCOMPLETE_TOKEN:
                break;
            case JsonTokenType::COMMENT:
                m_ColumnNumber += token().size();
                break;
            case JsonTokenType::BLOCK_COMMENT:
            case JsonTokenType::WHITESPACE:
                addLinesAndColumns(m_LineNumber, m_ColumnNumber,
                                   countLinesAndColumns(token()));
                break;
            case JsonTokenType::NEWLINE:
                ++m_LineNumber;
                m_ColumnNumber = 1;
                break;
            case JsonTokenType::END_OF_FILE:
                break;
            }
        }
        return false;
    }

    JsonTokenType JsonTokenizer::tokenType() const
    {
        return m_TokenType;
    }

    std::string_view JsonTokenizer::token() const
    {
        return std::string_view(m_TokenStart, m_TokenEnd - m_TokenStart);
    }

    std::string JsonTokenizer::tokenString() const
    {
        auto view = token();
        return std::string(view.begin(), view.end());
    }

    const std::string& JsonTokenizer::fileName() const
    {
        return m_FileName;
    }

    size_t JsonTokenizer::lineNumber() const
    {
        return m_LineNumber;
    }

    size_t JsonTokenizer::columnNumber() const
    {
        return m_ColumnNumber;
    }

    size_t JsonTokenizer::chunkSize() const
    {
        return m_ChunkSize;
    }

    void JsonTokenizer::setChunkSize(size_t value)
    {
        // The chunk size must be at least the size of the largest legal
        // Unicode code point. The TextReader will fail if it encounters a
        // character consisting of more bytes than the chunk size.
        if (value < 4)
            YSON_THROW("Chunk size can't be less than 4.");
        m_ChunkSize = value;
    }

    bool JsonTokenizer::internalNext()
    {
        if (m_TokenType == JsonTokenType::END_OF_FILE)
            return false;
        if (m_NextToken != m_TokenStart)
        {
            m_TokenStart = m_NextToken;
            auto token = nextToken(std::string_view(m_TokenStart,
                                                    m_BufferEnd - m_TokenStart));
            if (!token.isIncomplete)
            {
                m_NextToken = m_TokenEnd = token.endOfToken;
                m_TokenType = token.tokenType;
                return true;
            }
            else
            {
                m_TokenType = JsonTokenType::INCOMPLETE_TOKEN;
                return true;
            }
        }
        else
        {
            bool isEndOfFile = !fillBuffer();
            if (!isEndOfFile || !m_Buffer.empty())
            {
                auto token = nextToken(
                        std::string_view(m_TokenStart, m_BufferEnd - m_TokenStart),
                        isEndOfFile);
                if (!token.isIncomplete)
                {
                    m_NextToken = m_TokenEnd = token.endOfToken;
                    m_TokenType = token.tokenType;
                    return true;
                }
                else
                {
                    m_TokenType = JsonTokenType::INCOMPLETE_TOKEN;
                    return true;
                }
            }
            else
            {
                m_TokenType = JsonTokenType::END_OF_FILE;
                return false;
            }
        }
    }

    bool JsonTokenizer::fillBuffer()
    {
        if (m_TokenStart != m_BufferEnd && m_TokenStart != m_BufferStart)
        {
            std::copy(m_TokenStart, m_BufferEnd, m_Buffer.begin());
            m_Buffer.resize(m_BufferEnd - m_TokenStart);
            m_TokenStart = m_TokenEnd = m_NextToken = m_BufferStart;
            m_BufferEnd = m_BufferStart + m_Buffer.size();
        }
        else if (m_TokenStart == m_BufferEnd)
        {
            m_Buffer.clear();
        }

        if (!m_TextReader->read(m_Buffer, m_ChunkSize))
            return false;

        m_BufferStart = m_TokenStart = m_TokenEnd = m_NextToken = m_Buffer.data();
        m_BufferEnd = m_Buffer.data() + m_Buffer.size();
        return true;
    }

    void JsonTokenizer::removeLineContinuations()
    {
        assert(m_TokenEnd - m_TokenStart >= 2);
        ++m_TokenStart;
        auto from = &m_Buffer[0] + (m_TokenStart - m_Buffer.data());
        auto to = &m_Buffer[0] + (m_TokenEnd - m_Buffer.data()) - 1;
        auto next = findLineContinuation(from, to);
        auto dst = next.first;
        from = next.second;
        while (from != to)
        {
            next = findLineContinuation(from, to);
            std::copy(from, next.first, dst);
            dst += next.first - from;
            from = next.second;
        }
        m_TokenEnd = dst;
    }
}
