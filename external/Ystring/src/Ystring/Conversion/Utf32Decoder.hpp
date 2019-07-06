//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "AbstractDecoder.hpp"
#include "Ystring/Encodings/DecodeUtf32.hpp"

namespace Ystring { namespace Conversion {

    template <bool SwapBytes>
    class Utf32Decoder : public AbstractDecoder
    {
    public:
        Utf32Decoder()
            : AbstractDecoder(IsBigEndian == SwapBytes
                              ? Encoding::UTF_32_LE
                              : Encoding::UTF_32_BE)
        {}

        std::pair<bool, const char*> checkString(
                const char* srcBeg,
                const char* srcEnd,
                bool sourceIsIncomplete) const
        {
            auto tail = (srcEnd - srcBeg) % 4;
            return std::make_pair(tail == 0 || sourceIsIncomplete,
                                  srcEnd - tail);
        }

    protected:
        DecoderResult_t doDecode(
                const char*& srcBeg, const char* srcEnd,
                char32_t*& dstBeg, char32_t* dstEnd) const
        {
            while (dstBeg != dstEnd)
            {
                char32_t tmp = *dstBeg;
                auto result = Encodings::nextUtf32CodePoint<SwapBytes>(
                        tmp, srcBeg, srcEnd);
                *dstBeg = tmp;
                if (result != DecoderResult::OK)
                    return result;
                ++dstBeg;
            }
            return DecoderResult::OK;
        }

        DecoderResult_t doDecode(
                const char32_t*& srcBeg, const char32_t* srcEnd,
                char32_t*& dstBeg, char32_t* dstEnd) const
        {
            while (dstBeg != dstEnd)
            {
                char32_t tmp = *dstBeg;
                auto result = Encodings::nextUtf32CodePoint<SwapBytes>(
                        tmp, srcBeg, srcEnd);
                *dstBeg = tmp;
                if (result != DecoderResult::OK)
                    return result;
                ++dstBeg;
            }
            return DecoderResult::OK;
        }

        void skipInvalidCharacter(
                const char*& srcBeg, const char* srcEnd) const
        {
            if (!Encodings::skipNextUtf32CodePoint(srcBeg, srcEnd))
                srcBeg = srcEnd;
        }

        void skipInvalidCharacter(
                const char32_t*& srcBeg, const char32_t* srcEnd) const
        {
            Encodings::skipNextUtf32CodePoint(srcBeg, srcEnd);
        }
    };

    typedef Utf32Decoder<IsLittleEndian> Utf32BEDecoder;
    typedef Utf32Decoder<IsBigEndian> Utf32LEDecoder;
}}
