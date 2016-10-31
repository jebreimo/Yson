//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <utility>
#include "../YsonDefinitions.hpp"

namespace Yson
{
    YSON_API std::pair<int64_t, bool> parseInteger(
            const char* first, const char* last, bool detectBase);
}
