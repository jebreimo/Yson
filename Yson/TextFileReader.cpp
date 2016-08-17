//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextFileReader.hpp"

namespace Yson
{
    TextFileReader::TextFileReader(
            const std::string& fileName,
            Ystring::Encoding_t destinationEncoding)
        : m_FileStream(fileName)
    {
        init(m_FileStream, Ystring::Encoding::UNKNOWN, destinationEncoding);
    }

    TextFileReader::TextFileReader(
            const std::string& fileName,
            Ystring::Encoding_t sourceEncoding,
            Ystring::Encoding_t destinationEncoding)
        : m_FileStream(fileName)
    {
        init(m_FileStream, sourceEncoding, destinationEncoding);
    }
}
