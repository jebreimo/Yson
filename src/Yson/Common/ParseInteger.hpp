//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <string_view>

namespace Yson
{
    bool parse(std::string_view str, char& value, bool detectBase = false);

    bool parse(std::string_view str, signed char& value, bool detectBase = false);

    bool parse(std::string_view str, short& value, bool detectBase = false);

    bool parse(std::string_view str, int& value, bool detectBase = false);

    bool parse(std::string_view str, long& value, bool detectBase = false);

    bool parse(std::string_view str, long long& value, bool detectBase = false);

    bool parse(std::string_view str, unsigned char& value, bool detectBase = false);

    bool parse(std::string_view str, unsigned short& value, bool detectBase = false);

    bool parse(std::string_view str, unsigned int& value, bool detectBase = false);

    bool parse(std::string_view str, unsigned long& value, bool detectBase = false);

    bool parse(std::string_view str, unsigned long long& value, bool detectBase = false);
}
