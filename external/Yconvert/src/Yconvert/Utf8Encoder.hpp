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

        size_t get_encoded_size(const char32_t* src, size_t src_size) override;

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) override;

        size_t encode(const char32_t* src, size_t src_size,
                      std::string& dst) override;
    };
}
