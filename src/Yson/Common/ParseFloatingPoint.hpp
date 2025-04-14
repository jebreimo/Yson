//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string_view>
#include <utility>
#include "Yson/YsonDefinitions.hpp"

namespace Yson
{
    YSON_API bool parse(std::string_view str, float& value);

    YSON_API bool parse(std::string_view str, double& value);

    YSON_API bool parse(std::string_view str, long double& value);
}
