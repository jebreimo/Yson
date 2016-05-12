//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <utility>
#include "YsonDefinitions.hpp"

namespace Yson
{
    YSON_API std::pair<long double, bool> parseLongDouble(
            const char* first, const char* last);

    YSON_API std::pair<double, bool> parseDouble(
            const char* first, const char* last);

    YSON_API std::pair<float, bool> parseFloat(
            const char* first, const char* last);
}
