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
        size_t skip_character(const void* src, size_t src_size) const final;

        std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const final;
    };
}
