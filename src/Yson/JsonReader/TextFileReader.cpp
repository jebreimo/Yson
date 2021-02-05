//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextFileReader.hpp"
#include "Yson/Common/ThrowYsonException.hpp"
#include "Yson/Common/GetUnicodeFileName.hpp"

namespace Yson
{
    TextFileReader::TextFileReader(
            const std::string& fileName,
            Yconvert::Encoding sourceEncoding)
        : m_FileStream(getUnicodeFileName(fileName), std::ios_base::binary)
    {
        if (!m_FileStream)
            YSON_THROW("Unable to open file: " + fileName);
        init(m_FileStream, sourceEncoding);
    }
}
