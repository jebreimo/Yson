//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Yson/ValueType.hpp"
#include "Yson/YsonDefinitions.hpp"
#include "StringView.hpp"

namespace Yson
{
    YSON_API ValueType getValueType(std::string_view str);

    YSON_API ValueType getValueType(const std::string& str);
}
