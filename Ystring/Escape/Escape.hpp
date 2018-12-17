//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-05-31.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "../Encoding.hpp"
#include "../PlatformDetails.hpp"
#include "../YstringException.hpp"

/** @file
  * @brief The function library for UTF-8 encoded strings.
  */

namespace Ystring
{
    /** @brief Returns a copy of @a str where control character etc. have
      *     been escaped.
      *
      * When escaping with backslashes the function does not use octal codes,
      * not even \\0 for value 0 as these are too easy to misinterpret.
      */
    YSTRING_API std::string escape(const std::string& str,
                                   bool escapeNonAscii = false);

    /** @brief Returns true if @a str has characters that will be unescaped
      *     if unescape is called with the same parameters.
      */
    YSTRING_API bool hasEscapedCharacters(const std::string& str);

    /** @brief Returns true if @a str has characters that will be escaped
      *     if escape is called with the same parameters.
      */
    YSTRING_API bool hasUnescapedCharacters(const std::string& str,
                                            bool escapeNonAscii = false);

    /** @brief Returns a copy of @a str where all escape sequences have been
      *     translated to the characters they represent.
      * @throws YstringException if @a str contains an invalid
      *     escape sequence.
      */
    YSTRING_API std::string unescape(const std::string& str);
}
