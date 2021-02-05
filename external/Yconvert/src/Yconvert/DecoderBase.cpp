//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "DecoderBase.hpp"

#include "Yconvert/ConversionException.hpp"

namespace Yconvert
{
    DecoderBase::DecoderBase(Encoding encoding)
        : m_Encoding(encoding),
          m_ErrorHandlingPolicy()
    {}

    Encoding DecoderBase::encoding() const
    {
        return m_Encoding;
    }

    ErrorPolicy DecoderBase::errorHandlingPolicy() const
    {
        return m_ErrorHandlingPolicy;
    }

    void DecoderBase::setErrorHandlingPolicy(ErrorPolicy value)
    {
        m_ErrorHandlingPolicy = value;
    }

    std::pair<size_t, size_t>
    DecoderBase::decode(const void* src, size_t srcSize,
                        char32_t* dst, size_t dstSize) const
    {
        size_t iSrc = 0, iDst = 0;
        auto cSrc = static_cast<const char*>(src);
        while (true)
        {
            auto size = doDecode(cSrc + iSrc, srcSize - iSrc,
                                 dst + iDst, dstSize - iDst);
            iSrc += size.first;
            iDst += size.second;
            if (iSrc == srcSize || iDst == dstSize)
                return {iSrc, iDst};

            switch (m_ErrorHandlingPolicy)
            {
            case ErrorPolicy::REPLACE:
                dst[iDst++] = REPLACEMENT_CHARACTER;
                iSrc += skipCharacter(cSrc + iSrc, srcSize - iSrc);
                break;
            case ErrorPolicy::THROW:
                throw ConversionException("Invalid character in input.", iDst);
            case ErrorPolicy::SKIP:
                iSrc += skipCharacter(cSrc, srcSize);
                break;
            }
        }
    }
}
