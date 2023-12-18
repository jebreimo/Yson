//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseFloatingPoint.hpp"

#include <fast_float.h>

namespace Yson
{
    bool parse(std::string_view str, float& value)
    {
        return fast_float::from_chars(str.data(), str.data() + str.size(),
                                      value).ec == std::errc();
    }

    bool parse(std::string_view str, double& value)
    {
        return fast_float::from_chars(str.data(), str.data() + str.size(),
                                      value).ec == std::errc();
    }

    bool parse(std::string_view str, long double& value)
    {
        return fast_float::from_chars(str.data(), str.data() + str.size(),
                                      value).ec == std::errc();
    }
}
