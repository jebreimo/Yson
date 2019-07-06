//****************************************************************************
// Copyright © 2014 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2014-03-15.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Encoding.hpp"
#include "YstringDefinitions.hpp"
#include "Endianness.hpp"

/** @file
  * @brief Defines the EncodingInfo class and declares functions that can
  *     determine the encoding of a text.
  */

namespace Ystring
{
    /** @brief Provides various bits of information about an encoding.
      */
    class YSTRING_API EncodingInfo
    {
    public:
        /** @brief Sets encoding to UNKNOWN and initializes all other members
          *     to zero or empty strings.
          */
        EncodingInfo();

        /** @brief Initialzes members according to the function arguments.
          */
        EncodingInfo(Encoding_t encoding,
                     const std::string& name,
                     const std::string& byteOrderMark,
                     Endianness_t endianness,
                     size_t unitSize,
                     size_t minCharLength,
                     size_t maxCharLength);

        const std::string& byteOrderMark() const;
        void setByteOrderMark(const std::string& byteOrderMark);

        Endianness_t endianness() const;
        void setEndianness(Endianness_t endianness);

        size_t minCharLength() const;
        void setMinCharLength(size_t minCharLength);

        size_t maxCharLength() const;
        void setMaxCharLength(size_t maxCharLength);

        const std::string& name() const;
        void setName(const std::string& name);

        Encoding_t encoding() const;
        void setEncoding(Encoding_t encoding);

        size_t unitSize() const;
        void setUnitSize(size_t unitSize);
    private:
        std::string m_Name;
        std::string m_ByteOrderMark;
        size_t m_MinCharLength;
        size_t m_MaxCharLength;
        size_t m_UnitSize;
        Encoding_t m_Encoding;
        Endianness_t m_Endianness;
    };

    /** @brief Return the @a EncdingInfo for @a encoding.
      *
      * @return The instance of EncodingInfo corresponding to @a encoding or
      *     nullptr if there is no corresponding encoding.
      */
    YSTRING_API const EncodingInfo* getEncodingInfo(Encoding_t encoding);

    /** @brief Returns the @a Encoding_t that corresponds to @a name.
      */
    YSTRING_API Encoding_t encodingFromName(const std::string& name);

    /** @brief Checks the list of known byte-order marks and returns the one
      *     that matches the start of @a bom.
      */
    YSTRING_API Encoding_t determineEncodingFromByteOrderMark(
            const std::string& bom);

    /** @brief Checks the list of known byte-order marks and returns the one
      *     that matches the start of @a bom.
      */
    YSTRING_API Encoding_t determineEncodingFromByteOrderMark(
            const char* bom, size_t len);

    /** @brief Analyzes the contents of @a str and returns what it believes is
      *     most likely to @a str's encoding.
      */
    YSTRING_API Encoding_t determineEncodingFromContents(
            const char* str, size_t len,
            bool ignoreLastCharacter = false);

    /** @brief Returns the most likely encoding used in @a str.
      *
      * @note There's no guarantee that the encoding returned by this function
      *     actually matches the one used in str, only that str will consist
      *     of legal character values when interpreted as having the returned
      *     encoding.
      * @note If the length of @a str isn't divisible by 4, UTF-32 encodings
      *     will not be considered, and if it isn't divisible by 2, UTF-16 and
      *     other two-byte encodings will not be considered.
      */
    YSTRING_API Encoding_t determineEncodingFromContents(
            const std::string& str);

    /** @brief Returns the encoding used in @a stream and a pointer to the
      *     first character following the byte-order mark.
      *
      * The function first tries to determine the encoding based on
      * @a buffer's byte-order mark. If there isn't one, the contents of the
      * buffer is analyzed instead.
      */
    YSTRING_API std::pair<Encoding_t, const char*> determineEncoding(
            const char* buffer, size_t length);

    /** @brief Returns the encoding used in @a stream.
      *
      * Will look for a byte-order mark (bom) at the start of a stream. If one
      * is found it returns the corresponding encoding and leave the stream at
      * the first byte after the bom. If one isn't found it will read
      * @a maxScanLength number of bytes, do some basic analysis and try to
      * guess the encoding, and then reposition the stream back to its
      * original position.
      */
    YSTRING_API Encoding_t determineEncoding(std::istream& stream,
                                             size_t maxScanLength = 16);

}
