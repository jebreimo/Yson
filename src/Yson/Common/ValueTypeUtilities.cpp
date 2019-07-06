//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 17.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ValueTypeUtilities.hpp"

#include <cstdint>

namespace Yson
{
    int getDigit(char c)
    {
        return uint8_t(c) ^ 0x30;
    }

    bool isHexDigit(char c)
    {
        return (getDigit(c) <= 9) || ('A' <= c && c <= 'F')
               || ('a' <= c && c <= 'f');
    }
}
