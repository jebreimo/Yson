//****************************************************************************
// Copyright © 2011, Jan Erik Breimo.
// Created 2011-07-14 by Jan Erik Breimo
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <algorithm>
#include <cassert>

namespace Ystring { namespace Encodings
{
    namespace Detail
    {
        static inline bool isContinuation(uint8_t c)
        {
            return (c & 0xC0) == 0x80;
        }

        static inline bool isAscii(char32_t c)
        {
            return  (c & 0x80) == 0;
        }

        template<typename FwdIt>
        FwdIt findEndOfCodePoint(FwdIt begin, FwdIt end)
        {
            while (begin != end && isContinuation(*begin))
                ++begin;
            return begin;
        }
    }

    template<typename FwdIt>
    std::tuple<FwdIt, FwdIt, DecoderResult_t> nextInvalidUtf8CodePoint(
            FwdIt begin, FwdIt end)
    {
        for (auto it = begin; it != end; ++it)
        {
            if (Detail::isAscii(*it))
                continue;

            if (Detail::isContinuation(*it))
            {
                return std::make_tuple(
                        it, Detail::findEndOfCodePoint(it, end),
                        DecoderResult::INVALID);
            }

            auto first = it;
            auto bits = *it;
            auto bit = 0x40u;
            while (bit & bits)
            {
                if (++it == end)
                {
                    return std::make_tuple(
                            first, it, DecoderResult::INCOMPLETE);
                }
                if (!Detail::isContinuation(*it))
                    return std::make_tuple(first, it, DecoderResult::INVALID);
                bit >>= 1;
            }
            if (((bits & 0xFC) ^ 0xFC) < 0xC)
                return std::make_tuple(first, it, DecoderResult::INVALID);
        }

        return std::make_tuple(end, end, DecoderResult::OK);
    }

    template<typename FwdIt>
    bool isValidUtf8(FwdIt begin, FwdIt end, bool acceptIncomplete)
    {
        auto tuple = nextInvalidUtf8CodePoint(begin, end);
        auto result = std::get<2>(tuple);
        if (result == DecoderResult::OK)
            return true;
        return result == DecoderResult::OK
            || (result == DecoderResult::INCOMPLETE && acceptIncomplete);
    }

    template <typename FwdIt>
    DecoderResult_t nextUtf8CodePoint(char32_t& codePoint,
                                      FwdIt& it, FwdIt end)
    {
        if (it == end)
            return DecoderResult::END_OF_STRING;

        if (Detail::isAscii(*it))
        {
            codePoint = (uint8_t)*it++;
            return DecoderResult::OK;
        }

        if (Detail::isContinuation(*it) || uint8_t(*it) >= 0xFE)
            return DecoderResult::INVALID;

        size_t count = 1;
        unsigned bit = 0x20;
        codePoint = *it & 0x3F;
        while (codePoint & bit)
        {
            bit >>= 1;
            ++count;
        }

        codePoint &= bit - 1;

        FwdIt initialIt = it;
        while (++it != end && count && Detail::isContinuation(*it))
        {
            codePoint <<= 6;
            codePoint |= *it & 0x3F;
            --count;
        }

        if (count)
        {
            auto incomplete = it == end;
            it = initialIt;
            return incomplete ? DecoderResult::INCOMPLETE
                              : DecoderResult::INVALID;
        }

        return DecoderResult::OK;
    }

    template <typename BiIt>
    DecoderResult_t prevUtf8CodePoint(char32_t& codePoint,
                                      BiIt begin, BiIt& it)
    {
        if (it == begin)
            return DecoderResult::END_OF_STRING;

        BiIt initialIt = it;
        --it;
        if (Detail::isAscii(*it))
        {
            codePoint = (uint8_t)*it;
            return DecoderResult::OK;
        }

        codePoint = 0;
        uint8_t mask = 0xC0;
        uint8_t bit = 0x20;
        char32_t shift = 0;
        while (Detail::isContinuation(*it))
        {
            if (bit == 1 || it == begin)
            {
                it = initialIt;
                return DecoderResult::INVALID;
            }
            mask |= bit;
            bit >>= 1;
            codePoint |= char32_t(*it & 0x3F) << shift;
            shift += 6;
            --it;
        }

        uint8_t byte = *it;
        if ((byte & mask) != uint8_t(mask << 1))
        {
            it = initialIt;
            if ((byte & mask) != mask)
                return DecoderResult::INVALID;
            else
                return DecoderResult::INCOMPLETE;
        }

        codePoint |= (byte & ~mask) << shift;

        return DecoderResult::OK;
    }

    template <typename FwdIt>
    bool skipNextUtf8CodePoint(FwdIt& it, FwdIt end, size_t count)
    {
        for (auto i = 0u; i < count; ++i)
        {
            if (it == end)
                return false;
            do
            {
                ++it;
            }
            while (it != end && Detail::isContinuation(*it));
        }

        return true;
    }

    template <typename BiIt>
    bool skipPrevUtf8CodePoint(BiIt begin, BiIt& it, size_t count)
    {
        for (auto i = 0u; i < count; ++i)
        {
            if (it == begin)
                return false;
            do
            {
                --it;
            }
            while (it != begin && Detail::isContinuation(*it));
        }

        return true;
    }
}}
