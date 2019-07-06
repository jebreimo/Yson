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

#ifndef _NOEXCEPT
    #if (defined(_MSC_VER)) && (_MSC_VER < 1910)
        #define _NOEXCEPT
    #else
        #define _NOEXCEPT noexcept
    #endif
#endif

namespace Ystring
{
    class YSTRING_API YstringException : public std::logic_error
    {
    public:
        YstringException();
        explicit YstringException(const std::string& msg);
        YstringException(const std::string& msg,
                         const std::string& filename,
                         int lineno,
                         const std::string& funcname);
        const char* what() const _NOEXCEPT override;
    private:
        std::string m_Message;
    };
}

#define YSTRING_THROW(msg) \
    throw YstringException((msg), __FILE__, __LINE__, __FUNCTION__)
