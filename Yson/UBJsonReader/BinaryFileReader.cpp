//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "BinaryFileReader.hpp"
#include "../YsonException.hpp"
#include "../Common/GetUnicodeFileName.hpp"

namespace Yson
{
    BinaryFileReader::BinaryFileReader(const std::string& fileName)
        : m_Stream(getUnicodeFileName(fileName), std::ios_base::binary)
    {
        if (!m_Stream)
            YSON_THROW("Unable to open file: " + fileName);
        setStream(&m_Stream);
    }
}
