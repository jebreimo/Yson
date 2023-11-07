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

#ifdef YCONVERT_ENABLE_CODE_PAGES

namespace Yconvert
{
    struct CodePointMapRange
    {
        char32_t code_point;
        uint8_t index;
        uint8_t length;
    };

    class CodePageEncoder : public EncoderBase
    {
    public:
        CodePageEncoder(Encoding encoding,
                        const CodePageRange* ranges,
                        size_t ranges_size);

        void set_replacement_character(char32_t value) override;

        [[nodiscard]]
        size_t get_encoded_size(const char32_t* src, size_t src_size) override;

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) override;

        size_t encode(const char32_t* src, size_t src_size,
                      std::string& dst) override;

    private:
        std::vector<CodePointMapRange> ranges_;
    };
}

#endif