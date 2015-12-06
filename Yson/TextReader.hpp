//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Ystring/Encoding.hpp>
#include <Ystring/EncodingInfo.hpp>
#include <Ystring/Conversion/Converter.hpp>

namespace Yson {

    class TextReader
    {
    public:
        TextReader(std::istream& stream,
                   Ystring::Encoding_t destinationEncoding);

        TextReader(std::istream& stream,
                   Ystring::Encoding_t sourceEncoding,
                   Ystring::Encoding_t destinationEncoding);

        virtual ~TextReader();

        bool read(std::string& destination, size_t bytes);

//    bool read(std::u16string& destination, size_t bytes);
//
//    bool read(std::u32string& destination, size_t bytes);

    protected:
        TextReader();

        void init(std::istream& stream,
                  Ystring::Encoding_t sourceEncoding,
                  Ystring::Encoding_t destinationEncoding);

        void init(std::istream& stream,
                  Ystring::Encoding_t destinationEncoding);

    private:
        std::istream* m_Stream;
        std::unique_ptr<Ystring::Conversion::Converter> m_Converter;
        Ystring::Encoding_t m_DestinationEncoding;
        std::vector<char> m_Buffer;
        size_t m_UsedBufferSize;
    };
}
