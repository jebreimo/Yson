//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.12.2015
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
              m_DestinationEncoding(destinationEncoding),
              m_UsedBufferSize()
    {
    }

    TextReader::TextReader(std::istream& stream,
                           Encoding_t sourceEncoding,
                           Encoding_t destinationEncoding)
            : m_Stream(&stream),
              m_DestinationEncoding(destinationEncoding),
              m_UsedBufferSize()
    {
        if (sourceEncoding != Encoding::UNKNOWN)
        {
            m_Converter.reset(new Conversion::Converter(sourceEncoding,
                                                        destinationEncoding));
        }
    }

    TextReader::TextReader()
            : m_Stream(),
              m_DestinationEncoding(),
              m_UsedBufferSize()
    {
    }

    TextReader::~TextReader()
    {
    }

    bool TextReader::read(std::string& destination, size_t bytes)
    {
        m_Buffer.resize(m_UsedBufferSize + bytes);
        m_Stream->read(m_Buffer.data(), bytes);
        if (m_Stream->bad())
            return false;
        auto readBytes = m_Stream->gcount();
        if (readBytes == 0)
            return false;
        const char* bufferStart = m_Buffer.data();
        if (!m_Converter)
        {
            auto encoding = determineEncoding(m_Buffer.data(),
                                              std::min<size_t>(bytes, 256));
            bufferStart = encoding.second;
            readBytes -= bufferStart - m_Buffer.data();
            m_Converter.reset(new Conversion::Converter(
                    encoding.first, m_DestinationEncoding));
        }
        auto convertedBytes = m_Converter->convert(
                bufferStart, readBytes, destination);
        if (convertedBytes < readBytes)
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
        m_UsedBufferSize = 0;
    }

    void TextReader::init(std::istream& stream,
                          Encoding_t sourceEncoding,
                          Encoding_t destinationEncoding)
    {
        m_Stream = &stream;
        m_DestinationEncoding = destinationEncoding;
        m_Converter.reset(new Conversion::Converter(sourceEncoding,
                                                    destinationEncoding));
        m_UsedBufferSize = 0;
    }
}
