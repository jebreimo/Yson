//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <limits>
#include <string>

namespace Ystring { namespace Encodings
{
    template <bool SwapBytes, typename OutIt>
    OutIt addUtf16(OutIt out, char32_t codePoint);

    template <typename OutIt>
    OutIt addUtf16(OutIt out, char32_t codePoint);

    template <typename OutIt>
    OutIt addUtf16LE(OutIt out, char32_t codePoint);

    template <typename OutIt>
    OutIt addUtf16BE(OutIt out, char32_t codePoint);

    template <bool SwapBytes, typename OutIt>
    OutIt addUtf16AsBytes(OutIt out, char32_t codePoint);

    template <typename FwdIt>
    size_t encodeUtf16(FwdIt& begin, FwdIt end, char32_t codePoint);
    template <typename FwdIt>
    size_t encodeUtf16LE(FwdIt& begin, FwdIt end, char32_t codePoint);
    template <typename FwdIt>
    size_t encodeUtf16BE(FwdIt& begin, FwdIt end, char32_t codePoint);
}}

#include "EncodeUtf16-impl.hpp"
