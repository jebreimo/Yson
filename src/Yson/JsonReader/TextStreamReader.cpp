//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextStreamReader.hpp"

#include <algorithm>
#include <istream>
#include <Ystring/Conversion.hpp>
#include <Ystring/EncodingInfo.hpp>
#include "Yson/Common/DefaultBufferSize.hpp"

namespace Yson
{
    using namespace Ystring;

    TextStreamReader::TextStreamReader()
        : m_Stream()
    {
        m_Buffer.reserve(getDefaultBufferSize());
    }

    TextStreamReader::TextStreamReader(std::istream& stream,
                                       const char* buffer,
                                       size_t bufferSize,
                                       Encoding_t sourceEncoding)
        : m_Stream(&stream)
    {
        if (sourceEncoding != Encoding::UNKNOWN)
        {
            m_Converter.reset(new Conversion::Converter(
                    sourceEncoding, Ystring::Encoding::UTF_8));
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
        m_Stream->read(m_Buffer.data() + initialBufferSize, bytes);

        auto readBytes = static_cast<size_t>(m_Stream->gcount());
        if (readBytes < bytes)
            m_Buffer.resize(initialBufferSize + readBytes);

        if (m_Stream->bad() || readBytes == 0)
            return false;

        const char* bufferStart = m_Buffer.data();
        auto bufferSize = m_Buffer.size();
        if (!m_Converter)
        {
            auto encoding = determineEncoding(
                    m_Buffer.data(),
                    std::min<size_t>(bufferSize, 256));
            bufferStart = encoding.second;
            bufferSize -= bufferStart - m_Buffer.data();
            m_Converter.reset(new Conversion::Converter(
                    encoding.first, Ystring::Encoding::UTF_8));
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
                                Encoding_t sourceEncoding)
    {
        m_Stream = &stream;
        if (sourceEncoding != Encoding::UNKNOWN)
        {
            m_Converter.reset(new Conversion::Converter(
                    sourceEncoding, Ystring::Encoding::UTF_8));
        }
        else if (m_Converter)
        {
            m_Converter.reset();
        }
    }
}
