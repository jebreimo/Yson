//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 06.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <algorithm>
#include <cassert>

namespace Yson
{
    #ifdef IS_BIG_ENDIAN
    inline void fromBigEndian(size_t count, char* buffer)
    {}

    inline void fromBigEndian(size_t count, char* buffer, size_t unitSize)
    {}
    #else
    inline void fromBigEndian(size_t size, char* buffer)
    {
        std::reverse(buffer, buffer + size);
    }

    inline void fromBigEndian(size_t size, char* buffer, size_t unitSize)
    {
        assert(size % unitSize == 0);
        if (size == unitSize)
        {
            std::reverse(buffer, buffer + unitSize);
        }
        else
        {
            for (size_t i = unitSize; i <= size; i += unitSize)
                std::reverse(buffer + i - unitSize, buffer + i);
        }
    }
    #endif
}
