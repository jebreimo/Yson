//****************************************************************************
// Copyright © 2011, Jan Erik Breimo.
// Created 2011-07-14 by Jan Erik Breimo
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <cstddef>
#include "../YstringDefinitions.hpp"

/** @file
  * @brief Defines functions for working with UTF-8 encoded strings.
  */

namespace Ystring { namespace Encodings
{
    /** @brief The maximum length of an UTF-8-encoded code point.
      */
    static const size_t MAX_ENCODED_UTF8_LENGTH = 8;

    /** @brief Adds @a codePoint encoded as UTF-8 to @a it.
     *
     *  @return the new iterator position.
     */
    template <typename OutputIt>
    OutputIt addUtf8(OutputIt it, char32_t codePoint);

    /** @brief Encodes a unicode code point as UTF-8.
      *
      * @note The function does not add a terminating 0 to @a buffer.
      *
      * @param buffer a buffer for the UTF-8 code point, it must be at least
      *     as long as the encoding (i.e. 7 bytes to be error proof).
      * @param chr the code point to be encoded
      * @param bufferSize the size of @a buffer
      * @return the length of the encoded code point, or 0 if @a bufferSize is
      *     too small.
      */
    template <typename RndIt>
    size_t encodeUtf8(RndIt& begin, RndIt end, char32_t c);

    /** @brief Encodes a unicode code point as UTF-8.
     *  @return the length of the encoded code point.
     */
    YSTRING_API size_t encodeUtf8(char (&buffer)[MAX_ENCODED_UTF8_LENGTH],
                                  char32_t chr);

    YSTRING_API size_t utf8EncodedLength(char32_t c);
}}

#include "EncodeUtf8-impl.hpp"
