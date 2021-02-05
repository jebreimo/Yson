//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
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
        ErrorPolicy errorHandlingPolicy() const;

        void setErrorHandlingPolicy(ErrorPolicy value);

        std::pair<size_t, size_t>
        decode(const void* src, size_t srcSize,
               char32_t* dst, size_t dstSize) const;
    protected:
        explicit DecoderBase(Encoding encoding);

        virtual size_t
        skipCharacter(const void* src, size_t srcSize) const = 0;

        virtual std::pair<size_t, size_t>
        doDecode(const void* src, size_t srcSize,
                 char32_t* dst, size_t dstSize) const = 0;
    private:
        Encoding m_Encoding;
        ErrorPolicy m_ErrorHandlingPolicy;
    };
}
