//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextBufferReader.hpp"

#include <algorithm>
#include <Ystring/Conversion.hpp>
#include <Ystring/EncodingInfo.hpp>

namespace Yson
{
    namespace
    {
        size_t sizeWithoutIncompleteFinalCharacter(const char* str, size_t size);
    }

    TextBufferReader::TextBufferReader(const char* buffer, size_t size,
                                       Ystring::Encoding_t sourceEncoding)
            : m_Buffer(buffer),
              m_Size(size),
              m_Offset()
    {
        while (m_Size != 0 && m_Buffer[m_Size - 1] == 0)
            --m_Size;
        if (sourceEncoding != Ystring::Encoding::UNKNOWN)
        {
            m_Converter.reset(new Ystring::Conversion::Converter(
                    sourceEncoding, Ystring::Encoding::UTF_8));
        }
    }

    bool TextBufferReader::read(std::string& destination, size_t bytes)
    {
        bytes = std::min(m_Size - m_Offset, bytes);
        if (bytes == 0)
            return false;
        if (!m_Converter)
        {
            auto encoding = Ystring::determineEncoding(
                    m_Buffer,
                    std::min<size_t>(m_Size, 256));
            m_Offset = encoding.second - m_Buffer;
            m_Converter.reset(new Ystring::Conversion::Converter(
                    encoding.first,
                    Ystring::Encoding::UTF_8));
        }

        if (m_Converter->sourceEncoding() == Ystring::Encoding::UTF_8)
        {
            if (m_Offset + bytes != m_Size)
                bytes = sizeWithoutIncompleteFinalCharacter(m_Buffer + m_Offset, bytes);
            destination.insert(destination.end(),
                               m_Buffer + m_Offset,
                               m_Buffer + m_Offset + bytes);
        }
        else
        {
            bytes = m_Converter->convert(m_Buffer + m_Offset, bytes, destination);
        }
        m_Offset += bytes;
        return bytes != 0;
    }

    namespace
    {
        size_t sizeWithoutIncompleteFinalCharacter(const char* str, size_t size)
        {
            auto charSize = 0;
            for (auto i = size; i-- != 0;)
            {
                ++charSize;
                auto c = uint8_t(str[i]);
                if ((c & 0xC0) != 0x80)
                    break;
            }

            if (charSize < 2)
                return size;

            auto c = uint8_t(str[size - charSize]);
            switch (charSize)
            {
            case 2:
                return ((c & 0xE0) == 0xC0) ? size : size - 2;
            case 3:
                return ((c & 0xF0) == 0xE0) ? size : size - 3;
            case 4:
                return ((c & 0xF8) == 0xF0) ? size : size - 4;
            default:
                return size - charSize;
            }
        }
    }
}
