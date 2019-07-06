//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "AbstractEncoder.hpp"
#include "Ystring/Encodings/EncodeUtf32.hpp"

namespace Ystring { namespace Conversion {

    template<bool SwapBytes>
    class Utf32Encoder : public AbstractEncoder
    {
    public:
        Utf32Encoder()
            : AbstractEncoder(IsBigEndian == SwapBytes
                              ? Encoding::UTF_32_LE
                              : Encoding::UTF_32_BE)
        {}

    protected:
        bool doEncode(const char32_t*& srcBeg, const char32_t* srcEnd,
                      std::string& dst)
        {
            auto out = back_inserter(dst);
            while (srcBeg != srcEnd)
            {
                Encodings::addUtf32AsBytes<SwapBytes>(out, *srcBeg);
                ++srcBeg;
            }
            return true;
        }

        bool doEncode(const char32_t*& srcBeg, const char32_t* srcEnd,
                      std::u32string& dst)
        {
            auto out = back_inserter(dst);
            while (srcBeg != srcEnd)
            {
                Encodings::addUtf32<SwapBytes>(out, *srcBeg);
                ++srcBeg;
            }
            return true;
        }

#ifdef YSTRING_WCHAR_IS_4_BYTES

        bool doEncode(const char32_t*& srcBeg, const char32_t* srcEnd,
                      std::wstring& dst)
        {
            auto out = back_inserter(dst);
            while (srcBeg != srcEnd)
            {
                Encodings::addUtf32<SwapBytes>(out, *srcBeg);
                ++srcBeg;
            }
            return true;
        }

#endif

    };


    typedef Utf32Encoder<IsLittleEndian> Utf32BEEncoder;
    typedef Utf32Encoder<IsBigEndian> Utf32LEEncoder;

}}
