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
#include <Yconvert/Encoding.hpp>
#include "TextReader.hpp"
#include "Yson/YsonDefinitions.hpp"

namespace Yconvert
{
    class Converter;
}

namespace Yson
{
    class TextStreamReader : public TextReader
    {
    public:
        explicit TextStreamReader(
            std::istream& stream,
            const char* buffer = nullptr,
            size_t bufferSize = 0,
            Yconvert::Encoding sourceEncoding = Yconvert::Encoding::UNKNOWN);

        ~TextStreamReader() override;

        bool read(std::string& destination, size_t bytes) override;

    protected:
        TextStreamReader();

        void init(std::istream& stream, Yconvert::Encoding sourceEncoding);

    private:
        std::istream* m_Stream;
        std::unique_ptr<Yconvert::Converter> m_Converter;
        std::vector<char> m_Buffer;
    };
}
