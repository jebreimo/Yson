//****************************************************************************
// Copyright © 2018 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2018-12-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <vector>

namespace Yson
{
    size_t findSplitPos(std::string_view s, size_t suggestedPos);

    size_t getCurrentLineWidth(const std::string& buffer,
                               size_t maxLineWidth);
}
