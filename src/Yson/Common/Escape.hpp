//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-05-31.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include <string_view>

/** @file
  * @brief The function library for UTF-8 encoded strings.
  */

namespace Yson
{
    /** @brief Returns a copy of @a str where control character etc. have
      *     been escaped.
      *
      * When escaping with backslashes the function does not use octal codes,
      * not even \\0 for value 0 as these are too easy to misinterpret.
      */
    std::string escape(std::string_view str,
                       bool escapeNonAscii = false);

    /** @brief Returns true if @a str has characters that will be unescaped
      *     if unescape is called with the same parameters.
      */
    bool hasEscapedCharacters(std::string_view str);

    /** @brief Returns true if @a str has characters that will be escaped
      *     if escape is called with the same parameters.
      */
    bool hasUnescapedCharacters(std::string_view str,
                                bool escapeNonAscii = false);

    /** @brief Returns a copy of @a str where all escape sequences have been
      *     translated to the characters they represent.
      * @throws YstringException if @a str contains an invalid
      *     escape sequence.
      */
    std::string unescape(std::string_view str);
}
