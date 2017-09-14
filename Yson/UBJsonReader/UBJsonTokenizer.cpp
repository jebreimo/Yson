//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonTokenizer.hpp"
#include "BinaryBufferReader.hpp"
#include "BinaryFileReader.hpp"
#include "BinaryMutableBufferReader.hpp"
#include "UBJsonReaderException.hpp"
#include "UBJsonTokenizerUtilities.hpp"

namespace Yson
{
    UBJsonTokenizer::UBJsonTokenizer(std::istream& stream,
                                     const char* buffer,
                                     size_t bufferSize)
        : m_Reader(new BinaryStreamReader(stream, buffer, bufferSize))
    {}

    UBJsonTokenizer::UBJsonTokenizer(const std::string& fileName)
            : m_Reader(new BinaryFileReader(fileName)),
              m_FileName(fileName)
    {}

    UBJsonTokenizer::UBJsonTokenizer(char* buffer, size_t bufferSize)
            : m_Reader(new BinaryMutableBufferReader(buffer, bufferSize))
    {}

    UBJsonTokenizer::UBJsonTokenizer(const char* buffer, size_t bufferSize)
            : m_Reader(new BinaryBufferReader(buffer, bufferSize))
    {}

    size_t UBJsonTokenizer::contentSize() const
    {
        return m_ContentSize;
    }

    UBJsonTokenType UBJsonTokenizer::contentType() const
    {
        return m_ContentType;
    }

    std::string UBJsonTokenizer::fileName() const
    {
        return m_FileName;
    }

    bool UBJsonTokenizer::next()
    {
        while (true)
        {
            if (!m_Reader->read(1))
                return false;
            if (next(static_cast<UBJsonTokenType>(m_Reader->front())))
                return true;
        }
    }

    bool UBJsonTokenizer::next(UBJsonTokenType tokenType)
    {
        m_TokenType = tokenType;
        m_ContentSize =  0;
        switch (tokenType)
        {
        case UBJsonTokenType::INT8_TOKEN:
        case UBJsonTokenType::UINT8_TOKEN:
            if (m_Reader->read(1))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::INT16_TOKEN:
            if (m_Reader->read(2, 2))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::INT32_TOKEN:
            if (m_Reader->read(4, 4))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::INT64_TOKEN:
            if (m_Reader->read(8, 8))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::FLOAT32_TOKEN:
            if (m_Reader->read(4, 4))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::FLOAT64_TOKEN:
            if (m_Reader->read(8, 8))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::CHAR_TOKEN:
            if (m_Reader->read(1))
                return true;
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        case UBJsonTokenType::OBJECT_KEY_TOKEN:
            {
                char c;
                if (m_Reader->peek(&c) && c != '}')
                {
                    readSizedToken();
                    m_TokenType = UBJsonTokenType::STRING_TOKEN;
                    return true;
                }
                if (m_Reader->read(1))
                {
                    m_TokenType = UBJsonTokenType::END_OBJECT_TOKEN;
                    return true;
                }
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
            }
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
        case UBJsonTokenType::STRING_TOKEN:
            {
                readSizedToken();
                m_TokenType = tokenType;
                return true;
            }
        case UBJsonTokenType::START_OBJECT_TOKEN:
        case UBJsonTokenType::START_ARRAY_TOKEN:
            {
                char value;
                if (!m_Reader->peek(&value))
                    UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
                if (value == '$')
                {
                    if (!m_Reader->read(3))
                        UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
                    auto data = static_cast<const char*>(m_Reader->data());
                    if (data[2] != '#')
                        UBJSON_READER_THROW(
                                "Optimized object or array doesn't specify length.",
                                *this);
                    m_ContentType = static_cast<UBJsonTokenType>(data[1]);
                    if (!next())
                        UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
                    m_ContentSize = convertInteger<size_t>(m_TokenType,
                                                           m_Reader->data());
                    m_TokenType =
                            tokenType == UBJsonTokenType::START_ARRAY_TOKEN
                            ? UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN
                            : UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN;
                }
                else if (value == '#')
                {
                    m_Reader->read(1);
                    if (!next())
                        UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
                    m_ContentType = UBJsonTokenType::UNKNOWN_TOKEN;
                    m_ContentSize = convertInteger<size_t>(m_TokenType,
                                                           m_Reader->data());
                    m_TokenType =
                            tokenType == UBJsonTokenType::START_ARRAY_TOKEN
                            ? UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN
                            : UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN;
                }
                return true;
            }
        case UBJsonTokenType::NO_OP_TOKEN:
            return false;
        default:
            if (isValidTokenType(char(tokenType)))
                return true;
            UBJSON_READER_UNEXPECTED_TOKEN(*this);
        }
    }

    bool UBJsonTokenizer::read(void* buffer, size_t size,
                               UBJsonTokenType tokenType)
    {
        size_t unitSize;
        switch (tokenType)
        {
        case UBJsonTokenType::NULL_TOKEN:
            memset(buffer, 'Z', size);
            return true;
        case UBJsonTokenType::TRUE_TOKEN:
            memset(buffer, 'T', size);
            return true;
        case UBJsonTokenType::FALSE_TOKEN:
            memset(buffer, 'F', size);
            return true;
        case UBJsonTokenType::INT8_TOKEN:
        case UBJsonTokenType::UINT8_TOKEN:
        case UBJsonTokenType::CHAR_TOKEN:
            unitSize = 1;
            break;
        case UBJsonTokenType::INT16_TOKEN:
            unitSize = 2;
            break;
        case UBJsonTokenType::INT32_TOKEN:
        case UBJsonTokenType::FLOAT32_TOKEN:
            unitSize = 4;
            break;
        case UBJsonTokenType::INT64_TOKEN:
        case UBJsonTokenType::FLOAT64_TOKEN:
            unitSize = 8;
            break;
        default:
            return false;
        }
        if (m_Reader->read(buffer, size, unitSize))
            return true;
        UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
    }

    size_t UBJsonTokenizer::position() const
    {
        return m_Reader->position();
    }

    std::string_view UBJsonTokenizer::token() const
    {
        return std::string_view((const char*)m_Reader->data(),
                                m_Reader->size());
    }

    const void* UBJsonTokenizer::tokenData() const
    {
        return m_Reader->data();
    }

    size_t UBJsonTokenizer::tokenSize() const
    {
        return m_Reader->size();
    }

    UBJsonTokenType UBJsonTokenizer::tokenType() const
    {
        return m_TokenType;
    }

    void UBJsonTokenizer::readSizedToken()
    {
        if (!next())
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
        auto size = convertInteger<size_t>(m_TokenType, m_Reader->data());
        if (!m_Reader->read(size))
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(*this);
    }
}
