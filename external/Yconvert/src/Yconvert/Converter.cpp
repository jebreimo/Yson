//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Converter.hpp"

#include <cassert>
#include <cstring>
#include <vector>
#include "Yconvert/ConversionException.hpp"
#include "CodePageDecoder.hpp"
#include "CodePageEncoder.hpp"
#include "Utf32Decoder.hpp"
#include "Utf32Encoder.hpp"
#include "Utf8Decoder.hpp"
#include "Utf8Encoder.hpp"
#include "Utf16Decoder.hpp"
#include "Utf16Encoder.hpp"
#include "YconvertThrow.hpp"

namespace Yconvert
{
    namespace
    {
        const size_t DEFAULT_BUFFER_SIZE = 256;

        size_t find_nth_code_point(DecoderBase& decoder,
                                   const char* src, size_t src_size,
                                   std::vector<char32_t>& buf, size_t n)
        {
            assert(buf.size() >= n);
            if (n == 0)
                return 0;
            auto& info = get_encoding_info(decoder.encoding());
            if (info.max_units == 1)
                return n * info.unit_size;
            return decoder.decode(src, src_size, buf.data(), n).first;
        }

#ifdef YCONVERT_ENABLE_CODE_PAGES

        inline std::pair<const CodePageRange*, size_t>
        get_code_page_ranges(Encoding encoding)
        {
            static const CodePageRange ASCII_CHARS = {0x0000, 0, 127};
            if (encoding == Encoding::ASCII)
                return {&ASCII_CHARS, 1};
            #ifdef YCONVERT_ENABLE_ISO_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::ISO_8859_1)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_MAC_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::MAC_CYRILLIC)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_DOS_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::DOS_CP437)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_WIN_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::WIN_CP1250)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            return {nullptr, 0};
        }

        std::unique_ptr<DecoderBase> make_code_page_decoder(Encoding encoding)
        {
            auto [ranges, ranges_size] = get_code_page_ranges(encoding);
            if (ranges)
            {
                return std::unique_ptr<DecoderBase>(new CodePageDecoder(
                    encoding, ranges, ranges_size));
            }
            return {};
        }

        std::unique_ptr<EncoderBase> make_code_page_encoder(Encoding encoding)
        {
            auto [ranges, ranges_size] = get_code_page_ranges(encoding);
            if (ranges)
            {
                return std::unique_ptr<EncoderBase>(new CodePageEncoder(
                    encoding, ranges, ranges_size));
            }
            return {};
        }

#else

        std::unique_ptr<DecoderBase> make_code_page_decoder(Encoding)
        {
            return {};
        }

        std::unique_ptr<EncoderBase> make_code_page_encoder(Encoding)
        {
            return {};
        }

#endif

        std::unique_ptr<DecoderBase> make_decoder(Encoding encoding)
        {
            switch (encoding)
            {
            case Encoding::UTF_8:
                return std::unique_ptr<DecoderBase>(new Utf8Decoder);
            case Encoding::UTF_16_BE:
                return std::unique_ptr<DecoderBase>(new Utf16BEDecoder);
            case Encoding::UTF_16_LE:
                return std::unique_ptr<DecoderBase>(new Utf16LEDecoder);
            case Encoding::UTF_32_BE:
                return std::unique_ptr<DecoderBase>(new Utf32BEDecoder);
            case Encoding::UTF_32_LE:
                return std::unique_ptr<DecoderBase>(new Utf32LEDecoder);
            default:
                if (auto decoder = make_code_page_decoder(encoding))
                    return decoder;
                break;
            }

            const auto& info = get_encoding_info(encoding);
            YCONVERT_THROW("Unsupported decoder: " + std::string(info.name));
        }

        std::unique_ptr<EncoderBase> make_encoder(Encoding encoding)
        {
            switch (encoding)
            {
            case Encoding::UTF_8:
                return std::unique_ptr<EncoderBase>(new Utf8Encoder);
            case Encoding::UTF_16_BE:
                return std::unique_ptr<EncoderBase>(new Utf16BEEncoder);
            case Encoding::UTF_16_LE:
                return std::unique_ptr<EncoderBase>(new Utf16LEEncoder);
            case Encoding::UTF_32_BE:
                return std::unique_ptr<EncoderBase>(new Utf32BEEncoder);
            case Encoding::UTF_32_LE:
                return std::unique_ptr<EncoderBase>(new Utf32LEEncoder);
            default:
                if (auto encoder = make_code_page_encoder(encoding))
                    return encoder;
                break;
            }

            const auto& info = get_encoding_info(encoding);
            YCONVERT_THROW("Unsupported encoder: " + std::string(info.name));
        }

        template <unsigned UNIT_SIZE>
        size_t endian_copy(const void* src, size_t srcSize,
                           void* dst, size_t dstSize)
        {
            static_assert((UNIT_SIZE & ~UNIT_SIZE) == 0);
            size_t size = std::min(srcSize, dstSize);
            size -= size % UNIT_SIZE;
            auto csrc = static_cast<const char*>(src);
            auto cdst = static_cast<char*>(dst);
            if constexpr (UNIT_SIZE == 1)
            {
                memcpy(dst, src, size);
            }
            else
            {
                for (size_t i = 0; i < size; i += UNIT_SIZE)
                {
                    for (size_t j = 0; j < UNIT_SIZE; ++j)
                        cdst[i + j] = csrc[i + UNIT_SIZE - 1 - j];
                }
            }
            return size;
        }
    }

    Converter::Converter(Encoding src_encoding, Encoding dst_encoding)
        : decoder_(make_decoder(src_encoding)),
          encoder_(make_encoder(dst_encoding)),
          conversion_type_(get_conversion_type(src_encoding, dst_encoding))
    {}

    Converter::Converter(Converter&&) noexcept = default;

    Converter::~Converter() = default;

    Converter& Converter::operator=(Converter&&) noexcept = default;

    size_t Converter::buffer_size() const
    {
        return buffer_.empty() ? DEFAULT_BUFFER_SIZE : buffer_.size();
    }

    void Converter::set_buffer_size(size_t size)
    {
        buffer_.resize(size);
    }

    void Converter::set_error_policy(ErrorPolicy policy)
    {
        decoder_->set_error_policy(policy);
        encoder_->set_error_policy(policy);
    }

    ErrorPolicy Converter::error_policy() const
    {
        return decoder_->error_handling_policy();
    }

    char32_t Converter::replacement_character() const
    {
        return encoder_->replacement_character();
    }

    void Converter::set_replacement_character(char32_t value)
    {
        encoder_->set_replacement_character(value);
    }

    Encoding Converter::source_encoding() const
    {
        return decoder_->encoding();
    }

    Encoding Converter::destination_encoding() const
    {
        return encoder_->encoding();
    }

    size_t Converter::get_encoded_size(const void* src, size_t src_size)
    {
        if (conversion_type_ != ConversionType::CONVERT)
            return src_size;

        if (buffer_.empty())
            buffer_.resize(DEFAULT_BUFFER_SIZE);

        const auto& dec = get_encoding_info(decoder_->encoding());
        const auto& enc = get_encoding_info(encoder_->encoding());
        if (dec.max_units == 1 && enc.max_units == 1)
            return (src_size / dec.unit_size) * enc.unit_size;
        auto bytes = static_cast<const char*>(src);
        size_t size = 0;
        size_t offset = 0;
        for (;;)
        {
            auto [n, m] = decoder_->decode(bytes + offset, src_size - offset,
                                           buffer_.data(), buffer_.size());
            if (n == 0)
                break;
            offset += n;
            size += encoder_->get_encoded_size(buffer_.data(), m);
        }
        return size;
    }

    size_t Converter::convert(const void* src, size_t src_size,
                              std::string& dst)
    {
        switch (conversion_type_)
        {
        case ConversionType::SWAP_ENDIANNESS:
            {
                auto old_size = dst.size();
                dst.resize(old_size + src_size);
                return copy_and_swap(src, src_size,
                                     dst.data() + old_size, src_size);
            }
        case ConversionType::COPY:
            {
                auto old_size = dst.size();
                dst.resize(old_size + src_size);
                return copy(src, src_size, dst.data() + old_size, src_size);
            }
        case ConversionType::CONVERT:
            return do_convert(src, src_size, dst);
        default:
            return 0;
        }
    }

    std::pair<size_t, size_t>
    Converter::convert(const void* src, size_t src_size,
                       void* dst, size_t dst_size)
    {
        switch (conversion_type_)
        {
        case ConversionType::SWAP_ENDIANNESS:
        {
            auto n = copy_and_swap(src, src_size, dst, dst_size);
            return {n, n};
        }
        case ConversionType::COPY:
        {
            auto n = copy(src, src_size, dst, dst_size);
            return {n, n};
        }
        case ConversionType::CONVERT:
            return do_convert(src, src_size, dst, dst_size);
        default:
            return {0, 0};
        }
    }

    Converter::ConversionType Converter::get_conversion_type(
            Encoding src, Encoding dst)
    {
        if (src == dst)
            return ConversionType::COPY;
        if ((src == Encoding::UTF_16_LE && dst == Encoding::UTF_16_BE)
            || (src == Encoding::UTF_16_BE && dst == Encoding::UTF_16_LE)
            || (src == Encoding::UTF_32_LE && dst == Encoding::UTF_32_BE)
            || (src == Encoding::UTF_32_BE && dst == Encoding::UTF_32_LE))
        {
            return ConversionType::SWAP_ENDIANNESS;
        }
        return ConversionType::CONVERT;
    }

    size_t Converter::copy(const void* src, size_t src_size,
                           void* dst, size_t dst_size)
    {
        auto unit_size = get_encoding_info(encoder_->encoding()).unit_size;
        auto min_size = std::min(src_size, dst_size);
        min_size -= min_size % unit_size;
        memcpy(dst, src, min_size);
        return min_size;
    }

    size_t Converter::copy_and_swap(const void* src, size_t src_size,
                                    void* dst, size_t dst_size)
    {
        auto unit_size = get_encoding_info(decoder_->encoding()).unit_size;
        if (unit_size == 2)
            return endian_copy<2>(src, src_size, dst, dst_size);
        else if (unit_size == 4)
            return endian_copy<4>(src, src_size, dst, dst_size);
        return 0;
    }

    size_t Converter::do_convert(const void* src, size_t src_size,
                                 std::string& dst)
    {
        if (buffer_.empty())
            buffer_.resize(DEFAULT_BUFFER_SIZE);

        auto original_size = src_size;
        auto csrc = static_cast<const char*>(src);
        while (src_size != 0)
        {
            auto [m, n] = decoder_->decode(csrc, src_size,
                                           buffer_.data(), buffer_.size());
            auto p = encoder_->encode(buffer_.data(), n, dst);
            if (n != p)
            {
                src_size -= find_nth_code_point(*decoder_, csrc, src_size,
                                                buffer_, p);
                break;
            }
            csrc += m;
            src_size -= m;
        }
        return original_size - src_size;
    }

    std::pair<size_t, size_t>
    Converter::do_convert(const void* src, size_t src_size,
                          void* dst, size_t dst_size)
    {
        if (buffer_.empty())
            buffer_.resize(DEFAULT_BUFFER_SIZE);

        auto src_size_0 = src_size;
        auto dst_size_0 = dst_size;
        auto csrc = static_cast<const char*>(src);
        auto cdst = static_cast<char*>(dst);
        size_t num_code_points = 0;
        try
        {
            while (src_size != 0)
            {
                auto[m, n] = decoder_->decode(csrc, src_size,
                                              buffer_.data(), buffer_.size());
                auto [p, q] = encoder_->encode(buffer_.data(), n, cdst, dst_size);
                if (n != p)
                {
                    src_size -= find_nth_code_point(*decoder_, csrc, src_size,
                                                    buffer_, p);
                    break;
                }
                csrc += m;
                src_size -= m;
                cdst += q;
                dst_size -= q;
                num_code_points += n;
            }
        }
        catch (ConversionException& ex)
        {
            ex.code_point_index += num_code_points;
            throw;
        }
        return {src_size_0 - src_size, dst_size_0 - dst_size};
    }
}
