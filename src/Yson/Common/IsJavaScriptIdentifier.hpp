//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 15.08.2016.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string_view>
#include "Yson/YsonDefinitions.hpp"

namespace Yson
{
    YSON_API bool isJavaScriptIdentifier(const std::string_view& str);
}
