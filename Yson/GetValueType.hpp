//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "ValueType.hpp"
#include "YsonDefinitions.hpp"

namespace Yson
{
    YSON_API ValueType getValueType(const char* first, const char* last);

    YSON_API ValueType getValueType(
            const std::pair<const char*, const char*>& value);

    YSON_API ValueType getValueType(const std::string& value);
}
