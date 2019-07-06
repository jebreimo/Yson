//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "AbstractDecoder.hpp"

namespace Ystring { namespace Conversion {

    class Utf8Decoder : public AbstractDecoder
    {
    public:
        Utf8Decoder();

        std::pair<bool, const char*> checkString(
                const char* srcBeg,
                const char* srcEnd,
                bool sourceIsIncomplete) const;

    protected:
        DecoderResult_t doDecode(
                const char*& srcBeg, const char* srcEnd,
                char32_t*& dstBeg, char32_t* dstEnd) const;

        void skipInvalidCharacter(
                const char*& srcBeg, const char* srcEnd) const;
    };

}}
