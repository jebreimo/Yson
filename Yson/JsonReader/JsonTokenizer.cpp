//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.01.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizer.hpp"

#include <tuple>
#include "../YsonException.hpp"
#include "JsonTokenizerUtilities.hpp"
#include "TextBufferReader.hpp"
#include "TextFileReader.hpp"

namespace Yson
{
    JsonTokenizer::JsonTokenizer(std::istream& stream,
                                 const char* buffer,
                                 size_t bufferSize)
            : m_TextReader(new TextStreamReader(stream, buffer, bufferSize))
    {}

    JsonTokenizer::JsonTokenizer(const std::string& fileName)
            : m_TextReader(new TextFileReader(fileName)),
              m_FileName(fileName)
    {}

    JsonTokenizer::JsonTokenizer(const char* buffer, size_t bufferSize)
            : m_TextReader(new TextBufferReader(buffer, bufferSize))
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
            case JsonTokenType::STRING:
            case JsonTokenType::VALUE:
                m_ColumnNumber += m_TokenEnd - m_TokenStart;
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
        if (m_TokenType == JsonTokenType::STRING)
            return makeStringView(m_TokenStart + 1, m_TokenEnd - 1);
        else
            return makeStringView(m_TokenStart, m_TokenEnd);
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
        if (m_TokenEnd != m_TokenStart)
        {
            m_TokenStart = m_TokenEnd;
            auto token = nextToken(makeStringView(m_TokenStart, m_BufferEnd));
            if (!token.isIncomplete)
            {
                m_TokenEnd = token.endOfToken;
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
                        makeStringView(m_TokenStart, m_BufferEnd),
                        isEndOfFile);
                if (!token.isIncomplete)
                {
                    m_TokenEnd = token.endOfToken;
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
            m_TokenStart = m_TokenEnd = m_BufferStart;
            m_BufferEnd = m_BufferStart + m_Buffer.size();
        }
        else if (m_TokenStart == m_BufferEnd)
        {
            m_Buffer.clear();
        }

        if (!m_TextReader->read(m_Buffer, m_ChunkSize))
            return false;

        m_BufferStart = m_TokenStart = m_TokenEnd = m_Buffer.data();
        m_BufferEnd = m_Buffer.data() + m_Buffer.size();
        return true;
    }
}
