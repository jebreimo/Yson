//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-27
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Ystring/Encoding.hpp"
#include "Ystring/Conversion/ErrorHandlingPolicy.hpp"

namespace Ystring { namespace Conversion {

    class AbstractEncoder
    {
    public:
        virtual ~AbstractEncoder();

        Encoding_t encoding() const;

        const std::string& encodingName() const;

        ErrorHandlingPolicy_t errorHandlingPolicy() const;

        void setErrorHandlingPolicy(ErrorHandlingPolicy_t value);

        virtual char32_t replacementCharacter() const;

        virtual void setReplacementCharacter(char32_t value);

        bool encode(const char32_t*& srcBeg, const char32_t* srcEnd,
                    std::string& dst);

        bool encode(const char32_t*& srcBeg, const char32_t* srcEnd,
                    std::u16string& dst);

        bool encode(const char32_t*& srcBeg, const char32_t* srcEnd,
                    std::u32string& dst);

        bool encode(const char32_t*& srcBeg, const char32_t* srcEnd,
                    std::wstring& dst);

    protected:
        AbstractEncoder(Encoding_t encoding);

        virtual bool doEncode(
                const char32_t*& srcBeg, const char32_t* srcEnd,
                std::string& dst);

        virtual bool doEncode(
                const char32_t*& srcBeg, const char32_t* srcEnd,
                std::u16string& dst);

        virtual bool doEncode(
                const char32_t*& srcBeg, const char32_t* srcEnd,
                std::u32string& dst);

        virtual bool doEncode(
                const char32_t*& srcBeg, const char32_t* srcEnd,
                std::wstring& dst);

    private:
        Encoding_t m_Encoding;
        ErrorHandlingPolicy_t m_ErrorHandlingPolicy;
        char32_t m_ReplacementCharacter;
    };

}}
