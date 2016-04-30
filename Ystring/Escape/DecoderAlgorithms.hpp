//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-06-03
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <utility>
#include "../Encodings/DecodeUtf8.hpp"

namespace Ystring
{
    bool nextCodePoint(char32_t& codePoint,
                       std::string::const_iterator& it,
                       std::string::const_iterator last)
    {
        switch (Encodings::nextUtf8CodePoint(codePoint, it, last))
        {
        case DecoderResult::END_OF_STRING:
            return false;
        case DecoderResult::INCOMPLETE:
            YSTRING_THROW("Incomplete character.");
        case DecoderResult::INVALID:
            YSTRING_THROW("Invalid character.");
        default:
            break;
        }
        return true;
    }

    template <typename UnaryPred>
    bool advanceIfNot(std::string::const_iterator& it,
                      std::string::const_iterator last,
                      UnaryPred pred)
    {
        auto pos = it;
        char32_t ch;
        if (!nextCodePoint(ch, it, last) || pred(ch))
        {
            it = pos;
            return false;
        }
        return true;
    }

    template <typename UnaryPred>
    bool advanceUntil(std::string::const_iterator& it,
                      std::string::const_iterator last,
                      UnaryPred pred)
    {
        while (advanceIfNot(it, last, pred))
        {}
        return it != last;
    }
}
