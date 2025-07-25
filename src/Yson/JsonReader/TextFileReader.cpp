//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextFileReader.hpp"
#include "Yson/YsonException.hpp"

namespace Yson
{
    TextFileReader::TextFileReader(
            const std::filesystem::path& fileName,
            Yconvert::Encoding sourceEncoding)
        : m_FileStream(fileName, std::ios_base::binary)
    {
        if (!m_FileStream)
            YSON_THROW("Unable to open file: " + fileName.string());
        init(m_FileStream, sourceEncoding);
    }
}
