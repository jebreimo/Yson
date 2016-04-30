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
#include "../Ystring/Encoding.hpp"
#include "YsonDefinitions.hpp"

namespace Ystring { namespace Conversion {
    class Converter;
}}

namespace Yson {

    class YSON_API TextReader
    {
    public:
        TextReader(std::istream& stream,
                   Ystring::Encoding_t destinationEncoding);

        TextReader(std::istream& stream,
                   Ystring::Encoding_t sourceEncoding,
                   Ystring::Encoding_t destinationEncoding);

        virtual ~TextReader();

        bool read(std::string& destination, size_t bytes);

    protected:
        TextReader();

        void init(std::istream& stream,
                  Ystring::Encoding_t sourceEncoding,
                  Ystring::Encoding_t destinationEncoding);

    private:
        std::istream* m_Stream;
        std::unique_ptr<Ystring::Conversion::Converter> m_Converter;
        Ystring::Encoding_t m_DestinationEncoding;
        std::vector<char> m_Buffer;
    };
}
