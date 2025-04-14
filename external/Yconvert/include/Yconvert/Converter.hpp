//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-17.
//
// This file is distributed under the Zero-Clause BSD License.
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
        /** @brief Constructs a converter from @a src_encoding to
          *     @a dst_encoding.
          * @throw YconvertException if either of the encodings
          *     are unsupported.
          */
        Converter(Encoding src_encoding, Encoding dst_encoding);

        Converter(Converter&&) noexcept;

        Converter(const Converter&) = delete;

        ~Converter();

        Converter& operator=(Converter&&) noexcept;

        Converter& operator=(const Converter&) = delete;

        /** @brief Returns the size (in 32-bit code points) of the internal
          *     buffer used during conversion.
          */
        [[nodiscard]]
        size_t buffer_size() const;

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
        void set_buffer_size(size_t size);

        /** @brief Sets the error handling policies for both the decoder
          *     and encoder at once.
          */
        [[nodiscard]]
        ErrorPolicy error_policy() const;

        /** @brief Sets the error handling policies for both the decoder
          *     and encoder at once.
          */
        void set_error_policy(ErrorPolicy policy);

        /** @brief Returns the replacement character used by the decoder.
          *
          * The default value is the unicode "REPLACEMENT CHARACTER".
          */
        [[nodiscard]]
        char32_t replacement_character() const;

        /** @brief Set the replacement character used by both the decoder
          *     and encoder.
          *
          * The replacement character is used when the error handling policy
          * is REPLACE.
          */
        void set_replacement_character(char32_t value);

        /** @brief Returns the source encoding.
          */
        [[nodiscard]]
        Encoding source_encoding() const;

        /** @brief Returns the destination encoding.
          */
        [[nodiscard]]
        Encoding destination_encoding() const;

        [[nodiscard]]
        size_t get_encoded_size(const void* src, size_t src_size);

        size_t convert(const void* src, size_t src_size,
                       std::string& dst);

        std::pair<size_t, size_t> convert(const void* src, size_t src_size,
                                          void* dst, size_t dst_size);
    private:
        enum class ConversionType
        {
            CONVERT,
            COPY,
            SWAP_ENDIANNESS
        };

        static ConversionType get_conversion_type(Encoding src,
                                                  Encoding dst);

        size_t do_convert(const void* src, size_t src_size,
                          std::string& dst);

        std::pair<size_t, size_t> do_convert(const void* src, size_t src_size,
                                             void* dst, size_t dst_size);

        size_t copy(const void* src, size_t src_size,
                    void* dst, size_t dst_size);

        size_t copy_and_swap(const void* src, size_t src_size,
                             void* dst, size_t dst_size);

        std::unique_ptr<DecoderBase> decoder_;
        std::unique_ptr<EncoderBase> encoder_;
        ConversionType conversion_type_;
        std::vector<char32_t> buffer_;
    };
}
