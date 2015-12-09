//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <utility>

namespace Yson
{
    std::pair<double, bool> parseDouble(const char* first, const char* last);

    std::pair<float, bool> parseFloat(const char* first, const char* last);
}
