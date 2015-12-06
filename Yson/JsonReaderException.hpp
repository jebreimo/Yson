//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>

#ifndef _NOEXCEPT
    #define _NOEXCEPT
#endif

namespace Yson
{

    class JsonReaderException : public std::logic_error
    {
    public:

        JsonReaderException();

        JsonReaderException(const std::string& msg);

        JsonReaderException(const std::string& msg,
                            const std::string& sourceFileName,
                            int sourceFileLineNumber,
                            const std::string& funcName);

        JsonReaderException(const std::string& msg,
                            const std::string& sourceFileName,
                            int sourceFileLineNumber,
                            const std::string& funcName,
                            size_t fileLineNumber,
                            size_t fileColumnNumber);

        size_t lineNumber() const;
        void setLineNumber(size_t value);

        size_t columnNumber() const;
        void setColumnNumber(size_t value);

        const char* what() const _NOEXCEPT;

    private:
        std::string m_Message;
        size_t m_LineNumber;
        size_t m_ColumnNumber;
    };
}
