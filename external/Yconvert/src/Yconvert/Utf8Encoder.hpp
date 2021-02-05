//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-06
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "EncoderBase.hpp"

namespace Yconvert
{
    class Utf8Encoder : public EncoderBase
    {
    public:
        Utf8Encoder();

        size_t getEncodedSize(const char32_t* src, size_t srcSize) override;

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t srcSize,
               void* dst, size_t dstSize) override;

        size_t encode(const char32_t* src, size_t srcSize,
                      std::string& dst) override;
    };
}
