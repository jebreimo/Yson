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
#include <string_view>
#include "Yson/YsonDefinitions.hpp"

namespace Yson
{
    YSON_API std::string toBase64(const void* data, size_t size);

    YSON_API bool fromBase64(std::string_view text,
                             char* buffer, size_t& size);

    YSON_API bool fromBase64(std::string_view text,
                             std::vector<char>& buffer);

    YSON_API std::vector<char> fromBase64(std::string_view text);
}
