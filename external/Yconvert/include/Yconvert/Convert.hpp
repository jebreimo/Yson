//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2016-04-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Converter.hpp"
#include "Encoding.hpp"
#include <string>
#include <string_view>

/** @file
  * @brief Defines functions that convert strings from one encoding
  *     to another.
  */

namespace Yconvert
{
    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     *
     * @returns The number of bytes read from source and the number of bytes
     *  written to destination.
     */
    std::pair<size_t, size_t>
    convertString(const void* source, size_t sourceSize,
                  Encoding sourceEncoding,
                  void* destination, size_t destinationSize,
                  Encoding destinationEncoding,
                  ErrorPolicy errorPolicy);

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     *
     * @returns The number of values read from source and the number of
     *  bytes written to destination.
     */
    template <typename CharT>
    std::pair<size_t, size_t>
    convertString(std::basic_string_view<CharT> source,
                  Encoding sourceEncoding,
                  void* destination, size_t destinationSize,
                  Encoding destinationEncoding,
                  ErrorPolicy errorPolicy)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        using SrcString = std::basic_string_view<CharT>;
        auto srcSize = source.size() * sizeof(typename SrcString::value_type);
        auto n = converter.convert(source.data(), srcSize,
                                   destination, destinationSize);
        return {n.first / sizeof(typename SrcString::value_type),
                n.second};
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     */
    template <typename CharT>
    void convertString(std::basic_string_view<CharT> source,
                       Encoding sourceEncoding,
                       std::string& destination,
                       Encoding destinationEncoding,
                       ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        using SrcString = std::basic_string_view<CharT>;
        auto srcSize = source.size() * sizeof(typename SrcString::value_type);
        converter.convert(source.data(), srcSize, destination);
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     */
    template <typename Char1T, typename Char2T>
    void convertString(std::basic_string_view<Char1T> source,
                       Encoding sourceEncoding,
                       std::basic_string<Char2T>& destination,
                       Encoding destinationEncoding,
                       ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        auto srcSize = source.size() * sizeof(Char1T);
        auto dstSize = converter.getEncodedSize(source.data(), srcSize);
        destination.resize(dstSize / sizeof(Char2T));
        converter.convert(source.data(), srcSize,
                          destination.data(), dstSize);
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and returns the result to @a destination.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convertTo(std::basic_string_view<CharT> source,
                      Encoding sourceEncoding,
                      Encoding resultEncoding,
                      ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        StringT result;
        convertString<CharT, typename StringT::value_type>(
            source, sourceEncoding, result, resultEncoding, errorPolicy);
        return result;
    }

    template <typename StringT, typename CharT>
    StringT convertTo(const std::basic_string<CharT>& source,
                      Encoding sourceEncoding,
                      Encoding resultEncoding,
                      ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        return convertTo<StringT>(std::basic_string_view<CharT>(source),
                                  sourceEncoding,
                                  resultEncoding,
                                  errorPolicy);
    }
}
