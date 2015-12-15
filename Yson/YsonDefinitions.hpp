//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-15.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>

/** @file
  * @brief Defines constants and macros shared by all the other files
  *     in @a Yson.
  */

/// @cond

#ifdef _WIN32
    // Disabling warnings about exposing std::string, std::vector and
    // std::logic_error through the external DLL interface. The warnings are
    // valid, but it would defeat much of the purpose of Yson to remove
    // their cause (i.e. stop using std::string etc.).
    // The issue is instead solved by having separate debug and release
    // versions of the DLL and LIB files.
    #pragma warning(disable: 4251 4275)
    #ifdef YSON_EXPORTS
        #define YSON_API __declspec(dllexport)
    #else
        #define YSON_API
        #ifndef YSON_NO_AUTO_IMPORT
            #ifdef _DEBUG
                #pragma comment (lib, "Yson.debug.lib")
            #else
                #pragma comment (lib, "Yson.lib")
            #endif
        #endif
    #endif
#else
    #define YSON_API
#endif

/// @endcond

/** @brief The top-level namespace for all functions and classes
  *     in @a Yson.
  */
namespace Yson {}
