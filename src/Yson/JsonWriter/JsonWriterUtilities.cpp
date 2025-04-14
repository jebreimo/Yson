//****************************************************************************
// Copyright © 2018 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2018-12-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonWriterUtilities.hpp"

namespace Yson
{
    size_t findSplitPos(std::string_view s, size_t suggestedPos)
    {
        if (suggestedPos >= s.size() - 2)
            return s.size();
        auto pos = suggestedPos;
        if ((s[pos] & 0xC0) == 0x80)
        {
            for (++pos; pos != s.size(); ++pos)
            {
                if ((s[pos] & 0xC0) != 0x80)
                    break;
            }
            return pos;
        }
        if (s[pos - 1] == '\\')
        {
            size_t backslashCount = 1;
            for (--pos; pos-- > 0;)
            {
                if (s[pos] != '\\')
                {
                    backslashCount = suggestedPos - pos - 1;
                    break;
                }
            }
            if (backslashCount & 1)
                return suggestedPos - 1;
        }
        return suggestedPos;
    }

    size_t getCurrentLineWidth(const std::string& buffer,
                               size_t maxLineWidth)
    {
        auto maxOffset = std::min(buffer.size(), maxLineWidth);
        for (auto i = maxOffset; i-- > 0;)
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
                return maxOffset - i - 1;
        }
        return maxOffset;
    }
}
