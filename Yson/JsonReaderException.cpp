//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.12.2015
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonReaderException.hpp"

#include <sstream>

namespace Yson
{

    JsonReaderException::JsonReaderException()
        : std::logic_error("Unspecified error.")
    {
    }

    JsonReaderException::JsonReaderException(const std::string& msg)
        : std::logic_error(msg)
    {
    }

    JsonReaderException::JsonReaderException(
            const std::string& msg,
            const std::string& sourceFileName,
            int sourceFileLineNumber,
            const std::string& funcName)
        : std::logic_error(msg)
    {
        std::stringstream ss;
        if (!funcName.empty())
            ss << funcName << "() in ";
        ss << sourceFileName << ":" << sourceFileLineNumber << ": " << msg;
        m_Message = ss.str();
    }


    JsonReaderException::JsonReaderException(
            const std::string& msg,
            const std::string& sourceFileName,
            int sourceFileLineNumber,
            const std::string& funcName,
            size_t fileLineNumber,
            size_t fileColumnNumber)
        : std::logic_error(msg),
          m_LineNumber(fileLineNumber),
          m_ColumnNumber(fileColumnNumber)
    {
        std::stringstream ss;
        if (!funcName.empty())
            ss << funcName << "() in ";
        ss << sourceFileName << ":" << sourceFileLineNumber << ": "
           << "On line " << fileLineNumber
           << ", column " << fileColumnNumber
           << ": " << msg;
        m_Message = ss.str();
    }

    const char* JsonReaderException::what() const _NOEXCEPT
    {
        if (!m_Message.empty())
            return m_Message.c_str();
        return std::exception::what();
    }
}
