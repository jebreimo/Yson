//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 15.08.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "IsJavaScriptIdentifier.hpp"
#include <cctype>
#include <string_view>
#include "ValueTypeUtilities.hpp"

namespace Yson
{
    bool isUnicodeEscapeSequence(const std::string_view& str)
    {
        return str.size() >= 6 && str[0] == '\\' && str[1] == 'u'
               && isHexDigit(str[2]) && isHexDigit(str[3])
               && isHexDigit(str[4]) && isHexDigit(str[5]);
    }

    bool isJavaScriptIdentifier(const std::string_view& str)
    {
        if (str.empty())
            return false;
        size_t i = 0;

        if (!isalpha(str[0]) && str[0] != '$' && str[0] != '_')
        {
            if (!isUnicodeEscapeSequence({str.data(), str.size()}))
                return false;
            i = 3;
        }

        for (++i; i < str.size(); ++i)
        {
            if (!isalnum(str[i]) && str[i] != '$' && str[i] != '_')
            {
                if (!isUnicodeEscapeSequence(str.substr(i)))
                    return false;
                i += 3;
            }
        }
        return true;
    }
}
