//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 20.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>
#include <string>
#include <utility>

namespace Yson
{
    #ifndef _NOEXCEPT
        #if (defined(_MSC_VER)) && (_MSC_VER < 1910)
            #define _NOEXCEPT
        #else
            #define _NOEXCEPT noexcept
        #endif
    #endif

    /** @brief The base class for all exceptions thrown in the Yson library.
      */
    class YsonException : public std::runtime_error
    {
    public:
        /**
         * @brief Passes @a message on to the base class.
         */
        explicit YsonException(const std::string& message,
                               const std::string& debugLocation) noexcept
            : std::runtime_error(debugLocation + ": " + message),
              message(message)
        {}

        std::string message;
    };
}

#define _YSON_DEBUG_LOCATION_3(file, line) \
    file ":" #line

#define _YSON_DEBUG_LOCATION_2(file, line) \
    _YSON_DEBUG_LOCATION_3(file, line)

#define YSON_DEBUG_LOCATION() \
    _YSON_DEBUG_LOCATION_2(__FILE__, __LINE__)

#define YSON_THROW(msg) \
    throw ::Yson::YsonException(msg, YSON_DEBUG_LOCATION())
