//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <string>
#include "Yconvert/Encoding.hpp"
#include "Yconvert/ErrorPolicy.hpp"

namespace Yconvert
{
    class DecoderBase
    {
    public:
        virtual ~DecoderBase() = default;

        [[nodiscard]]
        Encoding encoding() const;

        [[nodiscard]]
        ErrorPolicy error_handling_policy() const;

        void set_error_policy(ErrorPolicy policy);

        std::pair<size_t, size_t>
        decode(const void* src, size_t src_size,
               char32_t* dst, size_t dst_size) const;
    protected:
        explicit DecoderBase(Encoding encoding);

        virtual size_t
        skip_character(const void* src, size_t src_size) const = 0;

        virtual std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const = 0;
    private:
        Encoding encoding_;
        ErrorPolicy error_policy_ = ErrorPolicy::THROW;
    };
}
