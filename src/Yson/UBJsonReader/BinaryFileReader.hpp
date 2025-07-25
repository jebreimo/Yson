//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "BinaryStreamReader.hpp"
#include <filesystem>
#include <fstream>

namespace Yson
{
    class BinaryFileReader : public BinaryStreamReader
    {
    public:
        explicit BinaryFileReader(const std::filesystem::path& fileName);

    private:
        std::ifstream m_Stream;
    };
}
