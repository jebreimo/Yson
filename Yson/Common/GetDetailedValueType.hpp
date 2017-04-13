//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 08.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "../YsonDefinitions.hpp"
#include "../DetailedValueType.hpp"
#include "StringView.hpp"

namespace Yson
{
    YSON_API DetailedValueType getDetailedValueType(
            std::string_view str);

    YSON_API DetailedValueType getExtendedDetailedValueType(
            std::string_view str);
}
