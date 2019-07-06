//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-09-19.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "DefaultBufferSize.hpp"

namespace Yson
{
    namespace
    {
        size_t g_DefaultBufferSize = 1024 * 1024;
    }

    size_t getDefaultBufferSize()
    {
        return g_DefaultBufferSize;
    }

    void setDefaultBufferSize(size_t bufferSize)
    {
        g_DefaultBufferSize = bufferSize;
    }
}
