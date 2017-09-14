//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.09.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>
#include <string>
#include "YsonException.hpp"

namespace Yson
{
    /** @brief The exception class used by functions in JsonReader/UBJsonReader.
      */
    class YsonReaderException : public YsonException
    {
    public:
        YsonReaderException()
            : YsonException("Unspecified error."),
              m_LineNumber(0),
              m_ColumnNumber(0)
        {}

        explicit YsonReaderException(const std::string& msg,
                                     std::string fileName = std::string(),
                                     size_t lineNumber = 0,
                                     size_t columnNumber = 0)
            : YsonException(msg),
              m_FileName(move(fileName)),
              m_LineNumber(lineNumber),
              m_ColumnNumber(columnNumber)
        {
            m_DefaultMessage = makeDefaultMessage(msg);
        }

        YsonReaderException(const std::string& msg,
                            const std::string& debugFileName,
                            int debugLineNumber,
                            const std::string& debugFunctionName,
                            std::string fileName = "",
                            size_t lineNumber = 0,
                            size_t columnNumber = 0)
            : YsonException(msg, debugFileName, debugLineNumber, debugFunctionName),
              m_FileName(move(fileName)),
              m_LineNumber(lineNumber),
              m_ColumnNumber(columnNumber)
        {
            m_DefaultMessage = makeDefaultMessage(msg);
        }

        const char* what() const _NOEXCEPT override
        {
            if (!m_DefaultMessage.empty())
                return m_DefaultMessage.c_str();
            return YsonException::what();
        }

        const std::string& fileName() const _NOEXCEPT
        {
            return m_FileName;
        }

        size_t lineNumber() const _NOEXCEPT
        {
            return m_LineNumber;
        }

        size_t columnNumber() const _NOEXCEPT
        {
            return m_ColumnNumber;
        }
    private:
        std::string makeDefaultMessage(const std::string& msg) const
        {
            std::string result;
            if (!m_FileName.empty())
                result = "In " + m_FileName;
            if (m_LineNumber || m_ColumnNumber)
            {
                if (!result.empty())
                    result += " at";
                else
                    result = "At";
                if (m_LineNumber)
                    result += " line " + std::to_string(m_LineNumber);
                if (m_ColumnNumber)
                {
                    if (m_LineNumber)
                        result += " and";
                    result += " column " + std::to_string(m_ColumnNumber);
                }
            }
            if (!result.empty())
                result += ": " + msg;
            return result;
        }

        std::string m_DefaultMessage;
        std::string m_FileName;
        size_t m_LineNumber;
        size_t m_ColumnNumber;
    };
}
