//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.12.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonTokenType.hpp"
#include "../Common/StringView.hpp"

namespace Yson
{
    struct Result
    {
        Result(JsonTokenType tokenType, const char* endOfToken)
            : tokenType(tokenType),
              endOfToken(endOfToken),
              isIncomplete(false)
        {}

        Result(JsonTokenType tokenType,
               const char* endOfToken,
               bool isIncomplete)
            : tokenType(tokenType),
              endOfToken(endOfToken),
              isIncomplete(isIncomplete)
        {}

        JsonTokenType tokenType;
        const char* endOfToken;
        bool isIncomplete;
    };

    Result nextToken(std::string_view string,
                     bool isEndOfFile = false);

    std::pair<size_t, size_t> countLinesAndColumns(std::string_view string);

    void addLinesAndColumns(std::pair<size_t&, size_t&> linesAndColumns,
                            std::pair<size_t, size_t> addend);
}
