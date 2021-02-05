//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "DecoderBase.hpp"

#include "CodePageDefinitions.hpp"

namespace Yconvert
{
    class CodePageDecoder : public DecoderBase
    {
    public:
        CodePageDecoder(Encoding encoding,
                        const CodePageRange* ranges,
                        size_t rangesSize);
    protected:
        size_t skipCharacter(const void* src, size_t srcSize) const override;

        std::pair<size_t, size_t>
        doDecode(const void* src, size_t srcSize,
                 char32_t* dst, size_t dstSize) const override;
    private:
        char32_t m_Chars[256];
    };
}
