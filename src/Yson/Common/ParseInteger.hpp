//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <string_view>

namespace Yson
{
    bool parse(std::string_view str, char& value, bool detectBase = false);

    bool parse(std::string_view str, int8_t& value, bool detectBase = false);

    bool parse(std::string_view str, int16_t& value, bool detectBase = false);

    bool parse(std::string_view str, int32_t& value, bool detectBase = false);

    bool parse(std::string_view str, int64_t& value, bool detectBase = false);

    bool parse(std::string_view str, uint8_t& value, bool detectBase = false);

    bool parse(std::string_view str, uint16_t& value, bool detectBase = false);

    bool parse(std::string_view str, uint32_t& value, bool detectBase = false);

    bool parse(std::string_view str, uint64_t& value, bool detectBase = false);
}
