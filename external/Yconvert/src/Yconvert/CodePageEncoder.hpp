//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "EncoderBase.hpp"

#include <vector>
#include "CodePageDefinitions.hpp"

namespace Yconvert
{
    struct CodePointMapRange
    {
        char32_t codePoint;
        uint8_t index;
        uint8_t length;
    };

    class CodePageEncoder : public EncoderBase
    {
    public:
        CodePageEncoder(Encoding encoding,
                        const CodePageRange* ranges,
                        size_t rangesSize);

        void setReplacementCharacter(char32_t value) override;

        size_t getEncodedSize(const char32_t* src, size_t srcSize) override;

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t srcSize, void* dst, size_t dstSize) override;

        size_t encode(const char32_t* src, size_t srcSize, std::string& dst) override;

    private:
        std::vector<CodePointMapRange> m_Ranges;
    };
}
