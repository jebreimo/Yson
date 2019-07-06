//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <vector>
#include <Ystring/Encoding.hpp>
#include "TextReader.hpp"
#include "../YsonDefinitions.hpp"

namespace Ystring { namespace Conversion
{
    class Converter;
}}

namespace Yson
{
    class TextStreamReader : public TextReader
    {
    public:
        TextStreamReader(std::istream& stream,
                         const char* buffer = nullptr,
                         size_t bufferSize = 0,
                         Ystring::Encoding_t sourceEncoding = Ystring::Encoding::UNKNOWN);

        ~TextStreamReader();

        bool read(std::string& destination, size_t bytes) override;

    protected:
        TextStreamReader();

        void init(std::istream& stream, Ystring::Encoding_t sourceEncoding);

    private:
        std::istream* m_Stream;
        std::unique_ptr<Ystring::Conversion::Converter> m_Converter;
        std::vector<char> m_Buffer;
    };
}
