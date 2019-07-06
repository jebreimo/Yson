//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-27
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "AbstractEncoder.hpp"

#include "Ystring/YstringException.hpp"
#include "Ystring/EncodingInfo.hpp"
#include "Ystring/Unicode/UnicodeChars.hpp"

namespace Ystring { namespace Conversion {

    AbstractEncoder::AbstractEncoder(Encoding_t encoding)
        : m_Encoding(encoding),
          m_ErrorHandlingPolicy(),
          m_ReplacementCharacter(Unicode::REPLACEMENT_CHARACTER)
    {}

    AbstractEncoder::~AbstractEncoder()
    {}

    Encoding_t AbstractEncoder::encoding() const
    {
        return m_Encoding;
    }

    const std::string& AbstractEncoder::encodingName() const
    {
        auto info = getEncodingInfo(m_Encoding);
        if (info)
            return info->name();

        static std::string defaultName = "Unknown";
        return defaultName;
    }

    ErrorHandlingPolicy_t AbstractEncoder::errorHandlingPolicy() const
    {
        return m_ErrorHandlingPolicy;
    }

    void AbstractEncoder::setErrorHandlingPolicy(ErrorHandlingPolicy_t value)
    {
        m_ErrorHandlingPolicy = value;
    }

    char32_t AbstractEncoder::replacementCharacter() const
    {
        return m_ReplacementCharacter;
    }

    void AbstractEncoder::setReplacementCharacter(char32_t value)
    {
        m_ReplacementCharacter = value;
    }

    bool AbstractEncoder::encode(const char32_t*& srcBeg,
                                 const char32_t* srcEnd,
                                 std::string& dst)
    {
        return doEncode(srcBeg, srcEnd, dst);
    }

    bool AbstractEncoder::encode(const char32_t*& srcBeg,
                                 const char32_t* srcEnd,
                                 std::u16string& dst)
    {
        return doEncode(srcBeg, srcEnd, dst);
    }

    bool AbstractEncoder::encode(const char32_t*& srcBeg,
                                 const char32_t* srcEnd,
                                 std::u32string& dst)
    {
        return doEncode(srcBeg, srcEnd, dst);
    }

    bool AbstractEncoder::encode(const char32_t*& srcBeg,
                                 const char32_t* srcEnd,
                                 std::wstring& dst)
    {
        return doEncode(srcBeg, srcEnd, dst);
    }

    bool AbstractEncoder::doEncode(const char32_t*& srcBeg,
                                   const char32_t* srcEnd,
                                   std::string& dst)
    {
        YSTRING_THROW(encodingName()
                      + "-encoder doesn't support 8-bit strings.");
    }

    bool AbstractEncoder::doEncode(const char32_t*& srcBeg,
                                   const char32_t* srcEnd,
                                   std::u16string& dst)
    {
        YSTRING_THROW(encodingName()
                      + "-encoder doesn't support 16-bit strings.");
    }

    bool AbstractEncoder::doEncode(const char32_t*& srcBeg,
                                   const char32_t* srcEnd,
                                   std::u32string& dst)
    {
        YSTRING_THROW(encodingName()
                      + "-encoder doesn't support 32-bit strings.");
    }

    bool AbstractEncoder::doEncode(const char32_t*& srcBeg,
                                   const char32_t* srcEnd,
                                   std::wstring& dst)
    {
        YSTRING_THROW(encodingName()
                      + "-encoder doesn't support 32-bit strings.");
    }

}}
