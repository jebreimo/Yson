//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-10.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>

/** @file
  * @brief Defines constants and macros shared by all the other files
  *     in @a Yconvert.
  */

/// @cond

#ifndef YCONVERT_API
  #ifdef _WIN32
      // Disabling warnings about exposing std::string, std::vector and
      // std::logic_error through the external DLL interface. The warnings are
      // valid, but it would defeat much of the purpose of Yconvert to remove
      // their cause (i.e. stop using std::string etc.).
      // The issue is instead solved by having separate debug and release
      // versions of the DLL and LIB files.
      #pragma warning(disable: 4251 4275)
      #ifdef YCONVERT_EXPORTS
          #define YCONVERT_API __declspec(dllexport)
      #else
          #define YCONVERT_API
      #endif
  #else
      #define YCONVERT_API
  #endif
#endif

/// @endcond

/** @brief The top-level namespace for all functions and classes
  *     in @a Yconvert.
  */
namespace Yconvert
{
    constexpr char32_t REPLACEMENT_CHARACTER = 0xFFFDu;

    /** @brief The maximum value for unicode code points.
      */
    constexpr char32_t UNICODE_MAX = 0x10FFFF;

    /** @brief A value representing invalid code points.
      */
    constexpr char32_t INVALID_CHAR = 0xFFFFFFFFu;
}
