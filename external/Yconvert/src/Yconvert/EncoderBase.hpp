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
        virtual ~EncoderBase();

        [[nodiscard]]
        Encoding encoding() const;

        [[nodiscard]]
        ErrorPolicy errorHandlingPolicy() const;

        void setErrorHandlingPolicy(ErrorPolicy value);

        [[nodiscard]]
        virtual char32_t replacementCharacter() const;

        virtual void setReplacementCharacter(char32_t value);

        virtual
        size_t getEncodedSize(const char32_t* src, size_t srcSize) = 0;

        virtual std::pair<size_t, size_t>
        encode(const char32_t* src, size_t srcSize,
               void* dst, size_t dstSize) = 0;

        virtual size_t encode(const char32_t* src, size_t srcSize,
                              std::string& dst) = 0;
    protected:
        explicit EncoderBase(Encoding encoding);
    private:
        Encoding m_Encoding;
        ErrorPolicy m_ErrorHandlingPolicy;
        char32_t m_ReplacementCharacter;
    };
}
