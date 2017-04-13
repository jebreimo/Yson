//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 24.12.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <experimental/string_view>

namespace std
{
    using experimental::string_view;
}

namespace Yson
{
    inline std::string_view makeStringView(const char* from, const char* to)
    {
        return std::string_view(from, to - from);
    }

    inline std::string_view makeStringView(const std::string& str)
    {
        return std::string_view(str.data(), str.size());
    }
}
