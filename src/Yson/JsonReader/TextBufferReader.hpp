//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.10.2016.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <Yconvert/Encoding.hpp>
#include "TextReader.hpp"

namespace Yconvert
{
    class Converter;
}

namespace Yson
{
    class TextBufferReader : public TextReader
    {
    public:
        TextBufferReader(const char* buffer, size_t size,
                         Yconvert::Encoding sourceEncoding = Yconvert::Encoding::UNKNOWN);

        bool read(std::string& destination, size_t bytes) override;
    private:
        const char* m_Buffer;
        size_t m_Size;
        size_t m_Offset;
        std::unique_ptr<Yconvert::Converter> m_Converter;
    };
}
