//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>
#include "YsonDefinitions.hpp"

/** @file
  * @brief Defines the JsonReaderException class.
  */

#ifndef _NOEXCEPT
    #define _NOEXCEPT
#endif

namespace Yson
{
    /** @brief JsonReader throws instances of this class when it
      *   encounters errors.
      */
    class YSON_API JsonReaderException : public std::logic_error
    {
    public:
        /** @brief Constructs a JsonReaderException with an unspecific
          *   error message and no file location.
          */
        JsonReaderException();

        /** @brief Constructs a JsonReaderException with the given error
          *   message, but no file location.
          */
        JsonReaderException(const std::string& msg);

        /** @brief Constructs a JsonReaderException with the given error
          *   message.
          *
          * The error message will include a reference to the file, function
          * and line in the source code from where the exception was thrown.
          */
        JsonReaderException(const std::string& msg,
                            const std::string& sourceFileName,
                            int sourceFileLineNumber,
                            const std::string& funcName);

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
                            size_t fileColumnNumber);

        /** @brief Returns the line in the JSON text where the error
          *   was encountered.
          */
        size_t lineNumber() const;

        /** @brief Sets the line in the JSON text where the error
          *   was encountered.
          */
        void setLineNumber(size_t value);

        /** @brief Returns the column in the JSON text where the error
          *   was encountered.
          */
        size_t columnNumber() const;

        /** @brief Sets the column in the JSON text where the error
          *   was encountered.
          */
        void setColumnNumber(size_t value);

        /** @brief Returns the error message.
          */
        const char* what() const _NOEXCEPT;

    private:
        std::string m_Message;
        size_t m_LineNumber;
        size_t m_ColumnNumber;
    };
}
