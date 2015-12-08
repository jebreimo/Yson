//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextReader.hpp"
#include <istream>

namespace Yson {

    using namespace Ystring;

    TextReader::TextReader(std::istream& stream,
                           Encoding_t destinationEncoding)
            : m_Stream(&stream),
              m_DestinationEncoding(destinationEncoding)
    {}

    TextReader::TextReader(std::istream& stream,
                           Encoding_t sourceEncoding,
                           Encoding_t destinationEncoding)
            : m_Stream(&stream),
              m_DestinationEncoding(destinationEncoding)
    {
        if (sourceEncoding != Encoding::UNKNOWN)
        {
            m_Converter.reset(new Conversion::Converter(sourceEncoding,
                                                        destinationEncoding));
        }
    }

    TextReader::TextReader()
            : m_Stream(),
              m_DestinationEncoding()
    {}

    TextReader::~TextReader()
    {}

    bool TextReader::read(std::string& destination, size_t bytes)
    {
        auto initialBufferSize = m_Buffer.size();
        m_Buffer.resize(initialBufferSize + bytes);
        m_Stream->read(m_Buffer.data() + initialBufferSize, bytes);
        auto readBytes = m_Stream->gcount();
        if (readBytes < bytes)
            m_Buffer.resize(initialBufferSize + readBytes);

        if (m_Stream->bad())
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
                    encoding.first, m_DestinationEncoding));
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

    void TextReader::init(std::istream& stream,
                          Encoding_t destinationEncoding)
    {
        m_Stream = &stream;
        m_DestinationEncoding = destinationEncoding;
        m_Converter.reset();
    }

    void TextReader::init(std::istream& stream,
                          Encoding_t sourceEncoding,
                          Encoding_t destinationEncoding)
    {
        m_Stream = &stream;
        m_DestinationEncoding = destinationEncoding;
        m_Converter.reset(new Conversion::Converter(sourceEncoding,
                                                    destinationEncoding));
    }
}
