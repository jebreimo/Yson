﻿//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-09-19.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include "../YsonDefinitions.hpp"

namespace Yson
{
    YSON_API size_t getDefaultBufferSize();

    YSON_API void setDefaultBufferSize(size_t bufferSize);
}
