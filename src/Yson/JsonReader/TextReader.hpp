//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.10.2016.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>

namespace Yson
{
    class TextReader
    {
    public:
        virtual ~TextReader() = default;
        virtual bool read(std::string& destination, size_t bytes) = 0;
    };
}

