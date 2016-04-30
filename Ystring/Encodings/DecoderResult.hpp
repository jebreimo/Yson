//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-07-01.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Ystring
{
    struct DecoderResult
    {
        enum Type
        {
            OK = 0,
            END_OF_STRING = 1,
            INCOMPLETE = 2,
            INVALID = 4
        };
    };

    typedef DecoderResult::Type DecoderResult_t;
}
