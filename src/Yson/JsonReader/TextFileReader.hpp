//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <fstream>
#include "TextStreamReader.hpp"

namespace Yson
{
    class TextFileReader : public TextStreamReader
    {
    public:
        explicit TextFileReader(
            const std::string& fileName,
            Yconvert::Encoding sourceEncoding = Yconvert::Encoding::UNKNOWN);
    private:
        std::ifstream m_FileStream;
    };
}
