//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Utf8Decoder.hpp"
#include "../Encodings/DecodeUtf8.hpp"

namespace Ystring { namespace Conversion {

    Utf8Decoder::Utf8Decoder()
        : AbstractDecoder(Encoding::UTF_8)
    {}

    std::pair<bool, const char*> Utf8Decoder::checkString(
            const char* srcBeg,
            const char* srcEnd,
            bool sourceIsIncomplete) const
    {
        auto invalid = Encodings::nextInvalidUtf8CodePoint(srcBeg, srcEnd);
        if (std::get<2>(invalid) == DecoderResult::OK)
            return std::make_pair(true, srcEnd);
        if (std::get<2>(invalid) == DecoderResult::INCOMPLETE
            && sourceIsIncomplete)
        {
            return std::make_pair(true, std::get<0>(invalid));
        }
        return std::make_pair(false, std::get<0>(invalid));
    }

    DecoderResult_t Utf8Decoder::doDecode(const char*& srcBeg,
                                          const char* srcEnd,
                                          char32_t*& dstBeg,
                                          char32_t* dstEnd) const
    {
        while (dstBeg != dstEnd)
        {
            char32_t tmp = *dstBeg;
            auto result = Encodings::nextUtf8CodePoint(tmp, srcBeg, srcEnd);
            *dstBeg = tmp;
            if (result != DecoderResult::OK)
                return result;
            ++dstBeg;
        }
        return DecoderResult::OK;
    }

    void Utf8Decoder::skipInvalidCharacter(
            const char*& srcBeg, const char* srcEnd) const
    {
        Encodings::skipNextUtf8CodePoint(srcBeg, srcEnd);
    }

}}
