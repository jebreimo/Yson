//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 15.08.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "IsJavaScriptIdentifier.hpp"

namespace Yson
{
    bool isJavaScriptIdentifier(const std::string& str)
    {
        if (str.empty())
            return false;
        if (!isalpha(str[0]) && str[0] != '$' && str[0] != '_')
            return false;

        for (size_t i = 1; i < str.size(); ++i)
        {
            if (!isalnum(str[i]) && str[i] != '$' && str[i] != '_')
                return false;
        }
        return true;
    }
}
