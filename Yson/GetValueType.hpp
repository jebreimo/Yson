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

namespace Yson {

    ValueType_t getValueType(const char* first, const char* last);

    ValueType_t getValueType(
            const std::pair<const char*, const char*>& value);

    ValueType_t getValueType(const std::string& value);
}
