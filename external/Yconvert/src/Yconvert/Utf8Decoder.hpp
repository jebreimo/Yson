//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "DecoderBase.hpp"

namespace Yconvert
{
    class Utf8Decoder : public DecoderBase
    {
    public:
        Utf8Decoder();
    protected:
        size_t skipCharacter(const void* src, size_t srcSize) const final;

        std::pair<size_t, size_t>
        doDecode(const void* src, size_t srcSize,
                 char32_t* dst, size_t dstSize) const final;
    };
}
