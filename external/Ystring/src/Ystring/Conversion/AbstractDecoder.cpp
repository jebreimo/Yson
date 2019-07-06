//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "AbstractDecoder.hpp"

#include "Ystring/YstringException.hpp"
#include "Ystring/EncodingInfo.hpp"
#include "Ystring/Unicode/UnicodeChars.hpp"

namespace Ystring { namespace Conversion {

    AbstractDecoder::AbstractDecoder(Encoding_t encoding)
        : m_Encoding(encoding),
          m_ReplacementCharacter(Unicode::REPLACEMENT_CHARACTER),
          m_ErrorHandlingPolicy()
    {}

    AbstractDecoder::~AbstractDecoder()
    {}

    size_t AbstractDecoder::characterUnitSize() const
    {
        auto info = getEncodingInfo(m_Encoding);
        if (!info)
            YSTRING_THROW("Uknown encoding.");
        return info->unitSize();
    }

    Encoding_t AbstractDecoder::encoding() const
    {
        return m_Encoding;
    }

    const std::string& AbstractDecoder::encodingName() const
    {
        auto info = getEncodingInfo(m_Encoding);
        if (info)
            return info->name();

        static std::string defaultName = "Unknown";
        return defaultName;
    }

    ErrorHandlingPolicy_t AbstractDecoder::errorHandlingPolicy() const
    {
        return m_ErrorHandlingPolicy;
    }

    void AbstractDecoder::setErrorHandlingPolicy(ErrorHandlingPolicy_t value)
    {
        m_ErrorHandlingPolicy = value;
    }

    char32_t AbstractDecoder::replacementCharacter() const
    {
        return m_ReplacementCharacter;
    }

    void AbstractDecoder::setReplacementCharacter(char32_t value)
    {
        m_ReplacementCharacter = value;
    }

    std::pair<bool, const char*> AbstractDecoder::checkString(
            const char* /*srcBeg*/,
            const char* srcEnd,
            bool /*sourceIsIncomplete*/) const
    {
        return std::make_pair(true, srcEnd);
    }

    std::pair<bool, const char16_t*> AbstractDecoder::checkString(
            const char16_t* /*srcBeg*/,
            const char16_t* srcEnd,
            bool /*sourceIsIncomplete*/) const
    {
        return std::make_pair(true, srcEnd);
    }

    std::pair<bool, const char32_t*> AbstractDecoder::checkString(
            const char32_t* /*srcBeg*/,
            const char32_t* srcEnd,
            bool /*sourceIsIncomplete*/) const
    {
        return std::make_pair(true, srcEnd);
    }

    DecoderResult_t AbstractDecoder::decode(const char*& srcBeg,
                                            const char* srcEnd,
                                            char32_t*& dstBeg,
                                            char32_t* dstEnd,
                                            bool sourceIsIncomplete) const
    {
        return decodeImpl(srcBeg, srcEnd, dstBeg, dstEnd, sourceIsIncomplete);
    }

    DecoderResult_t AbstractDecoder::decode(const char16_t*& srcBeg,
                                            const char16_t* srcEnd,
                                            char32_t*& dstBeg,
                                            char32_t* dstEnd,
                                            bool sourceIsIncomplete) const
    {
        return decodeImpl(srcBeg, srcEnd, dstBeg, dstEnd, sourceIsIncomplete);
    }

    DecoderResult_t AbstractDecoder::decode(const char32_t*& srcBeg,
                                            const char32_t* srcEnd,
                                            char32_t*& dstBeg,
                                            char32_t* dstEnd,
                                            bool sourceIsIncomplete) const
    {
        return decodeImpl(srcBeg, srcEnd, dstBeg, dstEnd, sourceIsIncomplete);
    }

    template<typename CharT>
    DecoderResult_t AbstractDecoder::decodeImpl(const CharT*& srcBeg,
                                                const CharT* srcEnd,
                                                char32_t*& dstBeg,
                                                char32_t* dstEnd,
                                                bool sourceIsIncomplete) const
    {
        auto initialSrcBeg = srcBeg;
        while (true)
        {
            auto result = doDecode(srcBeg, srcEnd, dstBeg, dstEnd);
            if (result == DecoderResult::OK
                || result == DecoderResult::END_OF_STRING)
            {
                return DecoderResult::OK;
            }
            if (result == DecoderResult::INCOMPLETE && sourceIsIncomplete)
                return DecoderResult::INCOMPLETE;

            switch (m_ErrorHandlingPolicy)
            {
            case ErrorHandlingPolicy::REPLACE:
                *dstBeg++ = m_ReplacementCharacter;
                skipInvalidCharacter(srcBeg, srcEnd);
                break;
            case ErrorHandlingPolicy::STOP:
                return DecoderResult::INVALID;
            case ErrorHandlingPolicy::THROW:
                YSTRING_THROW("Invalid character starting at index "
                              + std::to_string(srcBeg - initialSrcBeg));
            case ErrorHandlingPolicy::SKIP:
                skipInvalidCharacter(srcBeg, srcEnd);
                break;
            }
        }
    }

    DecoderResult_t AbstractDecoder::doDecode(const char*& srcBeg,
                                              const char* srcEnd,
                                              char32_t*& dstBeg,
                                              char32_t* dstEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 8-bit strings.");
    }

    DecoderResult_t AbstractDecoder::doDecode(const char16_t*& srcBeg,
                                              const char16_t* srcEnd,
                                              char32_t*& dstBeg,
                                              char32_t* dstEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 16-bit strings.");
    }

    DecoderResult_t AbstractDecoder::doDecode(const char32_t*& srcBeg,
                                              const char32_t* srcEnd,
                                              char32_t*& dstBeg,
                                              char32_t* dstEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 32-bit strings.");
    }

    void AbstractDecoder::skipInvalidCharacter(
            const char*& srcBeg, const char* srcEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 8-bit strings.");
    }

    void AbstractDecoder::skipInvalidCharacter(
            const char16_t*& srcBeg, const char16_t* srcEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 16-bit strings.");
    }

    void AbstractDecoder::skipInvalidCharacter(
            const char32_t*& srcBeg, const char32_t* srcEnd) const
    {
        YSTRING_THROW(encodingName()
                      + "-decoder doesn't support 32-bit strings.");
    }
}}
