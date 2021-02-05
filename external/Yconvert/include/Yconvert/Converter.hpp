//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Encoding.hpp"
#include "YconvertDefinitions.hpp"
#include "ErrorPolicy.hpp"

/** @file
  * @brief Defines the Converter class.
  */

namespace Yconvert
{

    class DecoderBase;
    class EncoderBase;

    /** @brief Converts strings from one encoding to another.
      */
    class YCONVERT_API Converter
    {
    public:
        /** @brief Constructs a converter from @a sourceEncoding to
          *     @a destinationEncoding.
          * @throw YconvertException if either of the encodings
          *     are unsupported.
          */
        Converter(Encoding sourceEncoding, Encoding destinationEncoding);

        Converter(Converter&&) noexcept;

        Converter(const Converter&) = delete;

        ~Converter();

        Converter& operator=(Converter&&) noexcept;

        Converter& operator=(const Converter&) = delete;

        /** @brief Returns the size (in 32-bit code points) of the internal
          *     buffer used during conversion.
          */
        [[nodiscard]]
        size_t bufferSize() const;

        /** @brief Sets the size (in 32-bit code points) of the internal
          *     buffer used during conversion.
          *
          * A large buffer may improve performance when long texts are
          * converted, but should normally be left alone.
          * The buffer isn't used at all for some trivial conversions, e.g.
          * bid-endian to little-endian versions of the same encoding.
          *
          * The default size is 256 code points (i.e. 1024 bytes).
          */
        void setBufferSize(size_t value);

        /** @brief Sets the error handling policies for both the decoder
          *     and encoder at once.
          */
        [[nodiscard]]
        ErrorPolicy errorHandlingPolicy() const;

        /** @brief Sets the error handling policies for both the decoder
          *     and encoder at once.
          */
        void setErrorHandlingPolicy(ErrorPolicy value);

        /** @brief Returns the replacement character used by the decoder.
          *
          * The default value is the unicode "REPLACEMENT CHARACTER".
          */
        [[nodiscard]]
        char32_t replacementCharacter() const;

        /** @brief Set the replacement character used by both the decoder
          *     and encoder.
          *
          * The replacement character is used when the error handling policy
          * is REPLACE.
          */
        void setReplacementCharacter(char32_t value);

        /** @brief Returns the source encoding.
          */
        [[nodiscard]]
        Encoding sourceEncoding() const;

        /** @brief Returns the destination encoding.
          */
        [[nodiscard]]
        Encoding destinationEncoding() const;

        [[nodiscard]]
        size_t getEncodedSize(const void* src, size_t srcSize);

        size_t convert(const void* src, size_t srcSize,
                       std::string& dst);

        std::pair<size_t, size_t> convert(const void* src, size_t srcSize,
                                          void* dst, size_t dstSize);
    private:
        enum class ConversionType
        {
            CONVERT,
            COPY,
            SWAP_ENDIANNESS
        };

        static ConversionType getConversionType(Encoding srcEncoding,
                                                Encoding dstEncoding);

        size_t doConvert(const void* src, size_t srcSize,
                         std::string& dst);

        std::pair<size_t, size_t> doConvert(const void* src, size_t srcSize,
                                            void* dst, size_t dstSize);

        size_t copy(const void* src, size_t srcSize,
                    void* dst, size_t dstSize);

        size_t copyAndSwap(const void* src, size_t srcSize,
                           void* dst, size_t dstSize);

        std::unique_ptr<DecoderBase> m_Decoder;
        std::unique_ptr<EncoderBase> m_Encoder;
        ConversionType m_ConversionType;
        std::vector<char32_t> m_Buffer;
    };
}
