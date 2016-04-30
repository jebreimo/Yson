//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-06
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "AbstractEncoder.hpp"

namespace Ystring { namespace Conversion {

    class Utf8Encoder : public AbstractEncoder
    {
    public:
        Utf8Encoder();

    protected:
        bool doEncode(const char32_t*& srcBeg, const char32_t* srcEnd,
                      std::string& dst);
    };

}}
