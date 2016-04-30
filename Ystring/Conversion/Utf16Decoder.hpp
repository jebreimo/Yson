//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "AbstractDecoder.hpp"
#include "../Encodings/DecodeUtf16.hpp"

namespace Ystring { namespace Conversion {

    template <bool SwapBytes>
    class Utf16Decoder : public AbstractDecoder
    {
    public:
        Utf16Decoder()
            : AbstractDecoder(IsBigEndian == SwapBytes
                              ? Encoding::UTF_16_LE
                              : Encoding::UTF_16_BE)
        {}

        std::pair<bool, const char*> checkString(
                const char* srcBeg,
                const char* srcEnd,
                bool sourceIsIncomplete) const
        {
            auto invalid = Encodings::nextInvalidUtf16CodePoint<SwapBytes>(
                    srcBeg, srcEnd);
            auto decoderResult = std::get<2>(invalid);
            if (decoderResult == DecoderResult::OK)
                return std::make_pair(true, srcEnd);
            return std::make_pair(decoderResult == DecoderResult::INCOMPLETE
                                  && sourceIsIncomplete,
                                  std::get<0>(invalid));
        }

        std::pair<bool, const char16_t*> checkString(
                const char16_t* srcBeg,
                const char16_t* srcEnd,
                bool sourceIsIncomplete) const
        {
            auto invalid = Encodings::nextInvalidUtf16CodePoint<SwapBytes>(
                    srcBeg, srcEnd);
            auto decoderResult = std::get<2>(invalid);
            if (decoderResult == DecoderResult::OK)
                return std::make_pair(true, srcEnd);
            return std::make_pair(decoderResult == DecoderResult::INCOMPLETE
                                  && sourceIsIncomplete,
                                  std::get<0>(invalid));
        }

    protected:
        DecoderResult_t doDecode(
                const char*& srcBeg, const char* srcEnd,
                char32_t*& dstBeg, char32_t* dstEnd) const
        {
            while (dstBeg != dstEnd)
            {
                char32_t tmp = *dstBeg;
                auto result = Encodings::nextUtf16CodePoint<SwapBytes>(
                        tmp, srcBeg, srcEnd);
                *dstBeg = tmp;
                if (result != DecoderResult::OK)
                    return result;
                ++dstBeg;
            }
            return DecoderResult::OK;
        }

        DecoderResult_t doDecode(
                const char16_t*& srcBeg, const char16_t* srcEnd,
                char32_t*& dstBeg, char32_t* dstEnd) const
        {
            while (dstBeg != dstEnd)
            {
                char32_t tmp = *dstBeg;
                auto result = Encodings::nextUtf16CodePoint<SwapBytes>(
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
            Encodings::skipNextUtf16CodePoint<SwapBytes>(srcBeg, srcEnd);
        }

        void skipInvalidCharacter(
                const char16_t*& srcBeg, const char16_t* srcEnd) const
        {
            Encodings::skipNextUtf16CodePoint<SwapBytes>(srcBeg, srcEnd);
        }
    };

    typedef Utf16Decoder<IsLittleEndian> Utf16BEDecoder;
    typedef Utf16Decoder<IsBigEndian> Utf16LEDecoder;
}}
