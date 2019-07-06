//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-11
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include "Ystring/Utilities/Endian.hpp"
#include "DecoderResult.hpp"

namespace Ystring { namespace Encodings
{
    template <bool SwapBytes, typename FwdIt>
    DecoderResult_t nextUtf32CodePoint(char32_t& codePoint,
                                       FwdIt& it, FwdIt end);

    template <typename FwdIt>
    DecoderResult_t nextUtf32LECodePoint(char32_t& codePoint,
                                         FwdIt& it, FwdIt end)
    {
        return nextUtf32CodePoint<IsBigEndian>(codePoint, it, end);
    }

    template <typename FwdIt>
    DecoderResult_t nextUtf32BECodePoint(char32_t& codePoint,
                                         FwdIt& it, FwdIt end)
    {
        return nextUtf32CodePoint<IsLittleEndian>(codePoint, it, end);
    }

    template <bool SwapBytes, typename BiIt>
    DecoderResult_t prevUtf32CodePoint(char32_t& codePoint,
                                       BiIt begin, BiIt& it);

    template <typename BiIt>
    DecoderResult_t prevUtf32LECodePoint(char32_t& codePoint,
                                         BiIt begin, BiIt& it)
    {
        return prevUtf32CodePoint<IsBigEndian>(codePoint, begin, it);
    }

    template <typename BiIt>
    DecoderResult_t prevUtf32BECodePoint(char32_t& codePoint,
                                         BiIt begin, BiIt& it)
    {
        return prevUtf32CodePoint<IsLittleEndian>(codePoint, begin, it);
    }

    template <typename FwdIt>
    bool skipNextUtf32CodePoint(FwdIt& it, FwdIt end, size_t count = 1);

    template <typename FwdIt>
    bool skipNextUtf32LECodePoint(FwdIt& it, FwdIt end, size_t count = 1)
    {
        return skipNextUtf32CodePoint<IsBigEndian>(it, end, count);
    }

    template <typename FwdIt>
    bool skipNextUtf32BECodePoint(FwdIt& it, FwdIt end, size_t count = 1)
    {
        return skipNextUtf32CodePoint<IsLittleEndian>(it, end, count);
    }

    template <typename BiIt>
    bool skipPrevUtf32CodePoint(BiIt begin, BiIt& it, size_t count = 1);

    template <typename BiIt>
    bool skipPrevUtf32LECodePoint(BiIt begin, BiIt& it, size_t count)
    {
        return skipPrevUtf32CodePoint<IsBigEndian>(begin, it, count = 1);
    }

    template <typename BiIt>
    bool skipPrevUtf32BECodePoint(BiIt begin, BiIt& it, size_t count = 1)
    {
        return skipPrevUtf32CodePoint<IsLittleEndian>(begin, it, count);
    }
}}

#include "DecodeUtf32-impl.hpp"
