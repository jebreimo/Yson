//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <fstream>
#include "TextStreamReader.hpp"

namespace Yson
{
    class YSON_API TextFileReader : public TextStreamReader
    {
    public:
        TextFileReader(const std::string& fileName,
                       Ystring::Encoding_t sourceEncoding = Ystring::Encoding::UNKNOWN);
    private:
        std::ifstream m_FileStream;
    };
}
