//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-08-03.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>
#include "YstringDefinitions.hpp"

/** @file
  * @brief Defines the exception thrown by Ystring functions.
  */

namespace Ystring
{
    class YstringException : public std::runtime_error
    {
    public:
        YstringException() noexcept
            : std::runtime_error("Unspecified error.")
        {}

        /**
         * @brief Passes @a message on to the base class.
         */
        explicit YstringException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}

        explicit YstringException(const char* message) noexcept
            : std::runtime_error(message)
        {}
    };
}

#define _YSTRING_THROW_3(file, line, msg) \
    throw ::Ystring::YstringException(file ":" #line ": " msg)

#define _YSTRING_THROW_2(file, line, msg) \
    _YSTRING_THROW_3(file, line, msg)

#define YSTRING_THROW(msg) \
    _YSTRING_THROW_2(__FILE__, __LINE__, msg)
