//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>

namespace Yson
{
    class JsonValueError : public std::runtime_error
    {
    public:
        JsonValueError(const std::string& msg) : std::runtime_error(msg) {}
    };
}
