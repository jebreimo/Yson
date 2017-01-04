//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include "YsonException.hpp"

/** @file
  * @brief Defines the JsonReaderException class.
  */

namespace Yson
{
    /** @brief JsonReader throws instances of this class when it
      *   encounters errors.
      */
    class JsonReaderException : public YsonException
    {
    public:
        /** @brief Constructs a JsonReaderException with an unspecific
          *   error message and no file location.
          */
        JsonReaderException()
                : YsonException("Unspecified error."),
                  m_LineNumber(0),
                  m_ColumnNumber(0)
        {}

        /** @brief Constructs a JsonReaderException with the given error
          *   message, but no file location.
          */
        JsonReaderException(const std::string& msg)
                : YsonException(msg),
                  m_LineNumber(0),
                  m_ColumnNumber(0)
        {}

        /** @brief Constructs a JsonReaderException with the given error
          *   message.
          *
          * The error message will include a reference to the file, function
          * and line in the source code from where the exception was thrown.
          */
        JsonReaderException(const std::string& msg,
                        const std::string& sourceFileName,
                        int sourceFileLineNumber,
                        const std::string& funcName)
                : YsonException(msg),
                  m_LineNumber(0),
                  m_ColumnNumber(0)
        {
            #ifndef NDEBUG
            std::stringstream ss;
            if (!funcName.empty())
                ss << funcName << "() in ";
            ss << sourceFileName << ":" << sourceFileLineNumber << ": "
               << msg;
            m_Message = ss.str();
            #endif
        }

        /** @brief Constructs a JsonReaderException with the given error
          *   message.
          *
          * The error message will include a reference to the file, function
          * and line in the source code from where the exception was thrown,
          * as well as the line and column in the JSON text where the error
          * was encountered.
          */
        JsonReaderException(const std::string& msg,
                        const std::string& sourceFileName,
                        int sourceFileLineNumber,
                        const std::string& funcName,
                        size_t fileLineNumber,
                        size_t fileColumnNumber)
                : YsonException(msg),
                  m_LineNumber(fileLineNumber),
                  m_ColumnNumber(fileColumnNumber)
        {
            std::stringstream ss;
            #ifndef NDEBUG
            if (!funcName.empty())
                ss << funcName << "() in ";
            ss << sourceFileName << ":" << sourceFileLineNumber << ": ";
            #endif
            ss << "On line " << fileLineNumber << ", column "
               << fileColumnNumber << ": " << msg;
            m_Message = ss.str();
        }

        /** @brief Returns the line in the JSON text where the error
          *   was encountered.
          */
        size_t lineNumber() const
        {
            return m_LineNumber;
        }

        /** @brief Sets the line in the JSON text where the error
          *   was encountered.
          */
        void setLineNumber(size_t value)
        {
            m_LineNumber = value;
        }

        /** @brief Returns the column in the JSON text where the error
          *   was encountered.
          */
        size_t columnNumber() const
        {
            return m_ColumnNumber;
        }

        /** @brief Sets the column in the JSON text where the error
          *   was encountered.
          */
        void setColumnNumber(size_t value)
        {
            m_ColumnNumber = value;
        }

        /** @brief Returns the error message.
          */
        const char* what() const _NOEXCEPT
        {
            if (!m_Message.empty())
                return m_Message.c_str();
            return std::exception::what();
        }

    private:
        std::string m_Message;
        size_t m_LineNumber;
        size_t m_ColumnNumber;
    };

    #define YSON_READER_THROW(msg) \
        throw JsonReaderException((msg), __FILE__, __LINE__, __FUNCTION__)

}
