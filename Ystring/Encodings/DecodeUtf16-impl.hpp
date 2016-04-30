//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-08
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <type_traits>
#include "../Utilities/Union16.hpp"
#include "DecoderResult.hpp"

namespace Ystring { namespace Encodings
{

    namespace Detail
    {
        template<typename T>
        struct Utf16CharType
        {
            typedef T Type;
        };

        #define YSTRING_DEFINE_UTF16_CHAR_TYPE(type, internalType) \
            template <> \
            struct Utf16CharType<type> \
            { \
                typedef internalType Type; \
            }

        YSTRING_DEFINE_UTF16_CHAR_TYPE(char, uint8_t);
        YSTRING_DEFINE_UTF16_CHAR_TYPE(int8_t, uint8_t);
        YSTRING_DEFINE_UTF16_CHAR_TYPE(int16_t, char16_t);
        YSTRING_DEFINE_UTF16_CHAR_TYPE(char16_t, char16_t);
        YSTRING_DEFINE_UTF16_CHAR_TYPE(char32_t, char32_t);
        YSTRING_DEFINE_UTF16_CHAR_TYPE(int32_t, char32_t);

        #ifdef YSTRING_WCHAR_IS_2_BYTES
            YSTRING_DEFINE_UTF16_CHAR_TYPE(wchar_t, char16_t);
        #else
            YSTRING_DEFINE_UTF16_CHAR_TYPE(wchar_t, char32_t);
        #endif

        using Utilities::swapEndianness;

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char16_t& word, FwdIt& it, FwdIt end,
                                 uint8_t)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            Utilities::Union16 chr;
            chr.u8[SwapBytes ? 1 : 0] = *it++;

            if (it == end)
                return DecoderResult::INCOMPLETE;

            chr.u8[SwapBytes ? 0 : 1] = *it++;

            word = chr.u16;
            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char16_t& word, FwdIt& it, FwdIt end,
                                 char16_t)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            word = *it++;
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char16_t& word, FwdIt& it, FwdIt end,
                                 char32_t)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            auto word32 = *it++;
            if (word32 > 0xFFFFu)
                return DecoderResult::INVALID;

            word = static_cast<char16_t>(word32);
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char16_t& word, BiIt begin, BiIt& it,
                                 uint8_t)
        {
            if (it == begin)
                return DecoderResult::END_OF_STRING;

            Utilities::Union16 chr;
            chr.u8[SwapBytes ? 0 : 1] = uint8_t(*(--it));

            if (it == begin)
                return DecoderResult::INCOMPLETE;

            chr.u8[SwapBytes ? 1 : 0] = uint8_t(*(--it));

            word = chr.u16;
            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char16_t& word, BiIt begin, BiIt& it,
                                 char16_t)
        {
            if (it == begin)
                return DecoderResult::END_OF_STRING;

            word = *(--it);
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char16_t& word, BiIt begin, BiIt& it,
                                 char32_t)
        {
            if (it == begin)
                return DecoderResult::END_OF_STRING;

            auto word32 = *(--it);
            if (word32 > 0xFFFFu)
                return DecoderResult::INVALID;

            word = static_cast<char16_t>(word32);
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }
    }

    template <bool SwapBytes, typename FwdIt>
    DecoderResult_t nextUtf16CodePoint(char32_t& codePoint,
                                       FwdIt& it, FwdIt end)
    {
        typedef typename std::iterator_traits<FwdIt>::value_type CharType;
        typedef typename Detail::Utf16CharType<CharType>::Type Utf16Type;

        char16_t chr;
        auto first = it;
        auto res = Detail::nextWord<SwapBytes>(chr, it, end, Utf16Type());
        if (res != DecoderResult::OK)
        {
            it = first;
            return res;
        }

        if (chr < 0xD800 || 0xE000 <= chr)
        {
            codePoint = chr;
            return DecoderResult::OK;
        }

        if (0xDC00 <= chr)
        {
            it = first;
            return DecoderResult::INVALID;
        }

        codePoint = char32_t(chr & 0x3FF) << 10;

        res = Detail::nextWord<SwapBytes>(chr, it, end, Utf16Type());

        if (res != DecoderResult::OK)
        {
            if (res == DecoderResult::END_OF_STRING)
                res = DecoderResult::INCOMPLETE;
            it = first;
            return res;
        }

        if (chr < 0xDC00 || 0xE000 <= chr)
        {
            it = first;
            return DecoderResult::INVALID;
        }

        codePoint |= chr & 0x3FF;
        codePoint += 0x10000;

        return DecoderResult::OK;
    }

    template <bool SwapBytes, typename BiIt>
    DecoderResult_t prevUtf16CodePoint(char32_t& codePoint,
                                       BiIt begin, BiIt& it)
    {
        typedef typename std::iterator_traits<BiIt>::value_type CharType;
        typedef typename Detail::Utf16CharType<CharType>::Type Utf16Type;

        char16_t chr;
        auto last = it;
        auto res = Detail::prevWord<SwapBytes>(chr, begin, it, Utf16Type());
        if (res != DecoderResult::OK)
        {
            it = last;
            return res;
        }

        if (chr < 0xD800 || 0xE000 <= chr)
        {
            codePoint = chr;
            return DecoderResult::OK;
        }

        if (chr < 0xDC00)
        {
            it = last;
            return DecoderResult::INVALID;
        }

        codePoint = char32_t(chr) & 0x3FF;

        res = Detail::prevWord<SwapBytes>(chr, begin, it, Utf16Type());
        if (res != DecoderResult::OK)
        {
            if (res == DecoderResult::END_OF_STRING)
                return DecoderResult::INCOMPLETE;
            it = last;
            return res;
        }

        if (chr < 0xD800 || 0xDC00 <= chr)
        {
            it = last;
            return DecoderResult::INVALID;
        }

        codePoint |= (chr & 0x3FF) << 10;
        codePoint += 0x10000;

        return DecoderResult::OK;
    }

    template <bool SwapBytes, typename FwdIt>
    bool skipNextUtf16CodePoint(FwdIt& it, FwdIt end, size_t count)
    {
        typedef typename std::iterator_traits<FwdIt>::value_type CharType;
        typedef typename Detail::Utf16CharType<CharType>::Type Utf16Type;

        for (auto i = 0u; i < count; ++i)
        {
            char16_t chr;
            auto res = Detail::nextWord<SwapBytes>(chr, it, end, Utf16Type());
            if (res != DecoderResult::OK)
            {
                if (res == DecoderResult::END_OF_STRING)
                    return false;
                continue;
            }
            if (0xD800 <= chr && chr < 0xDC00)
                Detail::nextWord<SwapBytes>(chr, it, end, Utf16Type());
        }
        return true;
    }

    template <bool SwapBytes, typename BiIt>
    bool skipPrevUtf16CodePoint(BiIt begin, BiIt& it, size_t count)
    {
        typedef typename std::iterator_traits<BiIt>::value_type CharType;
        typedef typename Detail::Utf16CharType<CharType>::Type Utf16Type;

        for (auto i = 0u; i < count; ++i)
        {
            char16_t chr;
            auto res = Detail::prevWord<SwapBytes>(chr, begin, it,
                                                   Utf16Type());
            if (res != DecoderResult::OK)
            {
                if (res == DecoderResult::END_OF_STRING)
                    return false;
                continue;
            }
            if (0xDC00 <= chr && chr < 0xE000)
                Detail::prevWord<SwapBytes>(chr, begin, it, Utf16Type());
        }
        return true;
    }

    template<bool SwapBytes, typename FwdIt>
    std::tuple<FwdIt, FwdIt, DecoderResult_t> nextInvalidUtf16CodePoint(
            FwdIt begin, FwdIt end)
    {
        typedef typename std::iterator_traits<FwdIt>::value_type CharType;
        typedef typename Detail::Utf16CharType<CharType>::Type Utf16Type;
        while (begin != end)
        {
            char16_t word;
            auto first = begin;
            auto res = Detail::nextWord<SwapBytes>(word, begin, end,
                                                   Utf16Type());
            if (res != DecoderResult::OK)
                return std::make_tuple(first, begin, res);

            if (word < 0xD800 || 0xE000 <= word)
                continue;

            if (0xDC00 <= word)
                return std::make_tuple(first, begin, DecoderResult::INVALID);

            auto second = begin;
            res = Detail::nextWord<SwapBytes>(word, begin, end, Utf16Type());

            if (res != DecoderResult::OK)
            {
                if (res == DecoderResult::END_OF_STRING)
                    res = DecoderResult::INCOMPLETE;
                return std::make_tuple(first, begin, res);
            }

            if (word < 0xDC00 || 0xE000 <= word)
                return std::make_tuple(first, second, DecoderResult::INVALID);
        }

        return std::make_tuple(end, end, DecoderResult::OK);
    }

}}
