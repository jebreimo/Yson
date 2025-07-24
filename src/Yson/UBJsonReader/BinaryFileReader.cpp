//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "BinaryFileReader.hpp"
#include "Yson/YsonException.hpp"

namespace Yson
{
    BinaryFileReader::BinaryFileReader(const std::filesystem::path& fileName)
        : m_Stream(fileName, std::ios_base::binary)
    {
        if (!m_Stream)
            YSON_THROW("Unable to open file: " + fileName.string());
        setStream(&m_Stream);
    }
}
