//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-27
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Yconvert/Encoding.hpp"
#include "Yconvert/ErrorPolicy.hpp"

namespace Yconvert
{
    class EncoderBase
    {
    public:
        virtual ~EncoderBase() = default;

        [[nodiscard]]
        Encoding encoding() const;

        [[nodiscard]]
        ErrorPolicy error_policy() const;

        void set_error_policy(ErrorPolicy policy);

        [[nodiscard]]
        virtual char32_t replacement_character() const;

        virtual void set_replacement_character(char32_t value);

        [[nodiscard]]
        virtual
        size_t get_encoded_size(const char32_t* src, size_t src_size) = 0;

        virtual std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) = 0;

        virtual size_t encode(const char32_t* src, size_t src_size,
                              std::string& dst) = 0;
    protected:
        explicit EncoderBase(Encoding encoding);
    private:
        Encoding encoding_;
        ErrorPolicy error_policy_;
        char32_t replacement_character_;
    };
}
