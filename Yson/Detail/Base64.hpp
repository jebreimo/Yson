//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 29.05.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <vector>

namespace Yson
{
    std::string toBase64(const void* data, size_t size);
    std::vector<uint8_t> fromBase64(const std::string& text);
}
