//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextStreamReader.hpp"

#include <algorithm>
#include <istream>
#include <memory>
#include <Yconvert/Converter.hpp>
#include "Yson/Common/DefaultBufferSize.hpp"

namespace Yson
{
    TextStreamReader::TextStreamReader()
        : m_Stream()
    {
        m_Buffer.reserve(getDefaultBufferSize());
    }

    TextStreamReader::TextStreamReader(std::istream& stream,
                                       const char* buffer,
                                       size_t bufferSize,
                                       Yconvert::Encoding sourceEncoding)
        : m_Stream(&stream)
    {
        if (sourceEncoding != Yconvert::Encoding::UNKNOWN)
        {
            m_Converter = std::make_unique<Yconvert::Converter>(
                    sourceEncoding, Yconvert::Encoding::UTF_8);
        }
        m_Buffer.reserve(std::max(getDefaultBufferSize(), bufferSize));
        if (buffer && bufferSize)
            m_Buffer.insert(m_Buffer.end(), buffer, buffer + bufferSize);
    }

    TextStreamReader::~TextStreamReader() = default;

    bool TextStreamReader::read(std::string& destination, size_t bytes)
    {
        auto initialBufferSize = m_Buffer.size();
        m_Buffer.resize(initialBufferSize + bytes);
        m_Stream->read(m_Buffer.data() + initialBufferSize,
                       std::streamsize(bytes));

        auto readBytes = static_cast<size_t>(m_Stream->gcount());
        if (readBytes < bytes)
            m_Buffer.resize(initialBufferSize + readBytes);

        if (m_Stream->bad() || m_Buffer.empty())
            return false;

        const char* bufferStart = m_Buffer.data();
        auto bufferSize = m_Buffer.size();
        if (!m_Converter)
        {
            auto [encoding, offset] = Yconvert::determine_encoding(
                    m_Buffer.data(),
                    std::min<size_t>(bufferSize, 256));
            bufferStart += offset;
            bufferSize -= offset;
            m_Converter = std::make_unique<Yconvert::Converter>(
                    encoding, Yconvert::Encoding::UTF_8);
        }

        auto convertedBytes = m_Converter->convert(
                bufferStart, bufferSize, destination);
        if (convertedBytes == bufferSize)
        {
            m_Buffer.clear();
        }
        else
        {
            auto tailOffset =
                    (bufferStart + convertedBytes) - m_Buffer.data();
            m_Buffer.erase(m_Buffer.begin(), m_Buffer.begin() + tailOffset);
        }
        return true;
    }

    void TextStreamReader::init(std::istream& stream,
                                Yconvert::Encoding sourceEncoding)
    {
        m_Stream = &stream;
        if (sourceEncoding != Yconvert::Encoding::UNKNOWN)
        {
            m_Converter = std::make_unique<Yconvert::Converter>(
                    sourceEncoding, Yconvert::Encoding::UTF_8);
        }
        else if (m_Converter)
        {
            m_Converter.reset();
        }
    }
}
