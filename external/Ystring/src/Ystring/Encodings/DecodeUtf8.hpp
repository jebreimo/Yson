//****************************************************************************
// Copyright © 2011, Jan Erik Breimo.
// Created 2011-07-14 by Jan Erik Breimo
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <iterator>
#include <limits>
#include <tuple>
#include "DecoderResult.hpp"

/** @file
  * @brief Defines functions for working with UTF-8 encoded strings.
  */

namespace Ystring { namespace Encodings
{
    namespace Detail
    {
        static inline bool isContinuation(uint8_t c)
        {
            return (c & 0xC0u) == 0x80;
        }

        static inline bool isAscii(char32_t c)
        {
            return  (c & 0x80u) == 0;
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
                bit >>= 1u;
            }
            if (((bits & 0xFC) ^ 0xFC) < 0xC)
                return std::make_tuple(first, it, DecoderResult::INVALID);
        }

        return std::make_tuple(end, end, DecoderResult::OK);
    }

    template<typename FwdIt>
    bool isValidUtf8(FwdIt begin, FwdIt end, bool acceptIncomplete = false)
    {
        auto tuple = nextInvalidUtf8CodePoint(begin, end);
        auto result = std::get<2>(tuple);
        return result == DecoderResult::OK
               || (result == DecoderResult::INCOMPLETE && acceptIncomplete);
    }

    /** @brief Assigns to @a codePoint the code point starting at @a it.
      *
      * @note The function returns false both when it has reached the end of
      *     the string and when it has encountered an invalid byte. The caller
      *     must compare @a it with @a end to determine why the result
      *     is false.
      *
      * @param codePoint  is assigned the code point starting at @a it.
      * @param it  start of the code point. Has been advanced to the start of
      *     the next code point upon return, or one byte past the first
      *     invalid byte in the current code point.
      * @param end  the end of the string.
      */
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
            bit >>= 1u;
            ++count;
        }

        codePoint &= bit - 1;

        FwdIt initialIt = it;
        while (++it != end && count && Detail::isContinuation(*it))
        {
            codePoint <<= 6u;
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

    template <typename FwdIt>
    bool skipNextUtf8CodePoint(FwdIt& it, FwdIt end, size_t count = 1)
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
}}
