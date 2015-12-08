//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <fstream>
#include "TextReader.hpp"

namespace Yson {

    class TextFileReader : public TextReader
    {
    public:
        TextFileReader(const std::string& fileName,
                       Ystring::Encoding_t destinationEncoding);

        TextFileReader(const std::string& fileName,
                       Ystring::Encoding_t sourceEncoding,
                       Ystring::Encoding_t destinationEncoding);

    private:
        std::ifstream m_FileStream;
    };
}