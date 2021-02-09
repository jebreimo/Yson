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

        size_t findNthCodePoint(DecoderBase& decoder,
                                const char* src, size_t srcSize,
                                std::vector<char32_t>& buf, size_t n)
        {
            assert(buf.size() >= n);
            if (n == 0)
                return 0;
            auto& info = getEncodingInfo(decoder.encoding());
            if (info.maxUnits == 1)
                return n * info.unitSize;
            return decoder.decode(src, srcSize, buf.data(), n).first;
        }

#ifdef YCONVERT_ENABLE_CODE_PAGES

        inline std::pair<const CodePageRange*, size_t>
        getCodePageRanges(Encoding encoding)
        {
            static const CodePageRange ASCII_CHARS = {0x0000, 0, 127};
            if (encoding == Encoding::ASCII)
                return {&ASCII_CHARS, 1};
            #ifdef YCONVERT_ENABLE_ISO_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::ISO_8859_1)) != 0)
                return getIsoCodePageRanges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_MAC_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::MAC_CYRILLIC)) != 0)
                return getIsoCodePageRanges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_DOS_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::DOS_CP437)) != 0)
                return getIsoCodePageRanges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_WIN_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::WIN_CP1250)) != 0)
                return getIsoCodePageRanges(encoding);
            #endif
            return {nullptr, 0};
        }

        std::unique_ptr<DecoderBase> makeCodePageDecoder(Encoding encoding)
        {
            auto [ranges, rangesSize] = getCodePageRanges(encoding);
            if (ranges)
            {
                return std::unique_ptr<DecoderBase>(new CodePageDecoder(
                    encoding, ranges, rangesSize));
            }
            return {};
        }

        std::unique_ptr<EncoderBase> makeCodePageEncoder(Encoding encoding)
        {
            auto [ranges, rangesSize] = getCodePageRanges(encoding);
            if (ranges)
            {
                return std::unique_ptr<EncoderBase>(new CodePageEncoder(
                    encoding, ranges, rangesSize));
            }
            return {};
        }

#else

        std::unique_ptr<DecoderBase> makeCodePageDecoder(Encoding)
        {
            return {};
        }

        std::unique_ptr<EncoderBase> makeCodePageEncoder(Encoding)
        {
            return {};
        }

#endif

        std::unique_ptr<DecoderBase> makeDecoder(Encoding encoding)
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
                if (auto decoder = makeCodePageDecoder(encoding))
                    return decoder;
                break;
            }

            const auto& info = getEncodingInfo(encoding);
            YCONVERT_THROW("Unsupported decoder: " + std::string(info.name));
        }

        std::unique_ptr<EncoderBase> makeEncoder(Encoding encoding)
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
                if (auto encoder = makeCodePageEncoder(encoding))
                    return encoder;
                break;
            }

            const auto& info = getEncodingInfo(encoding);
            YCONVERT_THROW("Unsupported encoder: " + std::string(info.name));
        }

        template <unsigned UNIT_SIZE>
        size_t endianCopy(const void* src, size_t srcSize, void* dst, size_t dstSize)
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

    Converter::Converter(Encoding srcEncoding, Encoding dstEncoding)
        : m_Decoder(makeDecoder(srcEncoding)),
          m_Encoder(makeEncoder(dstEncoding)),
          m_ConversionType(getConversionType(srcEncoding, dstEncoding)),
          m_Buffer(DEFAULT_BUFFER_SIZE)
    {}

    Converter::Converter(Converter&&) noexcept = default;

    Converter::~Converter() = default;

    Converter& Converter::operator=(Converter&&) noexcept = default;

    size_t Converter::bufferSize() const
    {
        return m_Buffer.size();
    }

    void Converter::setBufferSize(size_t value)
    {
        m_Buffer.resize(value);
    }

    void Converter::setErrorHandlingPolicy(ErrorPolicy value)
    {
        m_Decoder->setErrorHandlingPolicy(value);
        m_Encoder->setErrorHandlingPolicy(value);
    }

    ErrorPolicy Converter::errorHandlingPolicy() const
    {
        return m_Decoder->errorHandlingPolicy();
    }

    char32_t Converter::replacementCharacter() const
    {
        return m_Encoder->replacementCharacter();
    }

    void Converter::setReplacementCharacter(char32_t value)
    {
        m_Encoder->setReplacementCharacter(value);
    }

    Encoding Converter::sourceEncoding() const
    {
        return m_Decoder->encoding();
    }

    Encoding Converter::destinationEncoding() const
    {
        return m_Encoder->encoding();
    }

    size_t Converter::getEncodedSize(const void* src, size_t srcSize)
    {
        if (m_ConversionType != ConversionType::CONVERT)
            return srcSize;
        const auto& dec = getEncodingInfo(m_Decoder->encoding());
        const auto& enc = getEncodingInfo(m_Encoder->encoding());
        if (dec.maxUnits == 1 && enc.maxUnits == 1)
            return (srcSize / dec.unitSize) * enc.unitSize;
        auto cSrc = static_cast<const char*>(src);
        size_t size = 0;
        size_t offset = 0;
        for (;;)
        {
            auto [n, m] = m_Decoder->decode(cSrc + offset, srcSize - offset,
                                            m_Buffer.data(), m_Buffer.size());
            if (n == 0)
                break;
            offset += n;
            size += m_Encoder->getEncodedSize(m_Buffer.data(), m);
        }
        return size;
    }

    size_t Converter::convert(const void* src, size_t srcSize, std::string& dst)
    {
        switch (m_ConversionType)
        {
        case ConversionType::SWAP_ENDIANNESS:
            {
                auto oldSize = dst.size();
                dst.resize(oldSize + srcSize);
                return copyAndSwap(src, srcSize, dst.data() + oldSize, srcSize);
            }
        case ConversionType::COPY:
            {
                auto oldSize = dst.size();
                dst.resize(oldSize + srcSize);
                return copy(src, srcSize, dst.data() + oldSize, srcSize);
            }
        case ConversionType::CONVERT:
            return doConvert(src, srcSize, dst);
        default:
            return 0;
        }
    }

    std::pair<size_t, size_t>
    Converter::convert(const void* src, size_t srcSize, void* dst, size_t dstSize)
    {
        switch (m_ConversionType)
        {
        case ConversionType::SWAP_ENDIANNESS:
        {
            auto n = copyAndSwap(src, srcSize, dst, dstSize);
            return {n, n};
        }
        case ConversionType::COPY:
        {
            auto n = copy(src, srcSize, dst, dstSize);
            return {n, n};
        }
        case ConversionType::CONVERT:
            return doConvert(src, srcSize, dst, dstSize);
        default:
            return {0, 0};
        }
    }

    Converter::ConversionType Converter::getConversionType(
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

    size_t Converter::copy(const void* src, size_t srcSize, void* dst, size_t dstSize)
    {
        auto unitSize = getEncodingInfo(m_Encoder->encoding()).unitSize;
        auto minSize = std::min(srcSize, dstSize);
        minSize -= minSize % unitSize;
        memcpy(dst, src, minSize);
        return minSize;
    }

    size_t Converter::copyAndSwap(const void* src, size_t srcSize,
                                  void* dst, size_t dstSize)
    {
        auto unitSize = getEncodingInfo(m_Decoder->encoding()).unitSize;
        if (unitSize == 2)
            return endianCopy<2>(src, srcSize, dst, dstSize);
        else if (unitSize == 4)
            return endianCopy<4>(src, srcSize, dst, dstSize);
        return 0;
    }

    size_t Converter::doConvert(const void* src, size_t srcSize,
                                std::string& dst)
    {
        auto originalSize = srcSize;
        auto csrc = static_cast<const char*>(src);
        while (srcSize != 0)
        {
            auto [m, n] = m_Decoder->decode(csrc, srcSize,
                                            m_Buffer.data(), m_Buffer.size());
            auto p = m_Encoder->encode(m_Buffer.data(), n, dst);
            if (n != p)
            {
                srcSize -= findNthCodePoint(*m_Decoder, csrc, srcSize,
                                            m_Buffer, p);
                break;
            }
            csrc += m;
            srcSize -= m;
        }
        return originalSize - srcSize;
    }

    std::pair<size_t, size_t>
    Converter::doConvert(const void* src, size_t srcSize,
                         void* dst, size_t dstSize)
    {
        auto srcSize0 = srcSize;
        auto dstSize0 = dstSize;
        auto csrc = static_cast<const char*>(src);
        auto cdst = static_cast<const char*>(dst);
        size_t numCodePoints = 0;
        try
        {
            while (srcSize != 0)
            {
                auto[m, n] = m_Decoder->decode(csrc, srcSize,
                                               m_Buffer.data(), m_Buffer.size());
                auto [p, q] = m_Encoder->encode(m_Buffer.data(), n, dst, dstSize);
                if (n != p)
                {
                    srcSize -= findNthCodePoint(*m_Decoder, csrc, srcSize,
                                                m_Buffer, p);
                    break;
                }
                csrc += m;
                srcSize -= m;
                cdst += q;
                dstSize -= q;
                numCodePoints += n;
            }
        }
        catch (ConversionException& ex)
        {
            ex.codePointIndex += numCodePoints;
            throw;
        }
        return {srcSize0 - srcSize, dstSize0 - dstSize};
    }
}
