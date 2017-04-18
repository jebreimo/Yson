//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "../../Ystring/Encoding.hpp"
#include "TextReader.hpp"

namespace Ystring { namespace Conversion
{
    class Converter;
}}

namespace Yson
{
    class TextBufferReader : public TextReader
    {
    public:
        TextBufferReader(const char* buffer, size_t size,
                         Ystring::Encoding_t sourceEncoding = Ystring::Encoding::UNKNOWN);

        bool read(std::string& destination, size_t bytes) override;
    private:
        const char* m_Buffer;
        size_t m_Size;
        size_t m_Offset;
        std::unique_ptr<Ystring::Conversion::Converter> m_Converter;
    };
}
