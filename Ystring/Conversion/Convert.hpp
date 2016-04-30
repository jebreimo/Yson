//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2016-04-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Converter.hpp"

/** @file
  * @brief Defines functions that convert strings from one encoding
  *     to another.
  */

namespace Ystring { namespace Conversion
{
    /** @brief Takes the string in @a source, converts it from
      *     @a sourceEncoding to @a destinationEncoding and writes the result
      *     to @a destination.
      */
    template <typename Char1T, typename Char2T>
    void convert(const std::basic_string<Char1T>& source,
                 Encoding_t sourceEncoding,
                 std::basic_string<Char2T>& destination,
                 Encoding_t destinationEncoding)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.convert(source, destination);
    }

    /** @brief Takes the string in @a source, converts it from
      *     @a sourceEncoding to @a destinationEncoding and writes the result
      *     to @a destination.
      */
    template <typename Char1T, typename Char2T>
    void convert(const std::basic_string<Char1T>& source,
                 Encoding_t sourceEncoding,
                 std::basic_string<Char2T>& destination,
                 Encoding_t destinationEncoding,
                 ErrorHandlingPolicy_t errorHandlingPolicy)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorHandlingPolicy);
        converter.convert(source, destination);
    }

    /** @brief Takes the string in @a source, converts it from
      *     @a sourceEncoding to @a destinationEncoding and returns
      *     the result.
      *
      * The first template parameter is the result's string type and must be
      * specified explicitly.
      */
    template <typename StringT, typename CharT>
    StringT convert(const std::basic_string<CharT>& source,
                    Encoding_t sourceEncoding,
                    Encoding_t resultEncoding)
    {
        StringT result;
        convert(source, sourceEncoding, result, resultEncoding);
        return result;
    }

    /** @brief Takes the string in @a source, converts it from
      *     @a sourceEncoding to @a destinationEncoding and returns
      *     the result.
      *
      * The first template parameter is the result's string type and must be
      * specified explicitly.
      */
    template <typename StringT, typename CharT>
    StringT convert(const std::basic_string<CharT>& source,
                    Encoding_t sourceEncoding,
                    Encoding_t resultEncoding,
                    ErrorHandlingPolicy_t errorHandlingPolicy)
    {
        StringT result;
        convert(source, sourceEncoding, result, resultEncoding,
                errorHandlingPolicy);
        return result;
    }

}}
