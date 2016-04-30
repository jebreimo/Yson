//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-11.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <type_traits>
#include "DecoderResult.hpp"
#include "../PlatformDetails.hpp"
#include "../Utilities/Iterators.hpp"
#include "../Utilities/Union32.hpp"

namespace Ystring { namespace Encodings
{
    typedef std::make_unsigned<wchar_t>::type UnsignedWChar;

    namespace Detail
    {
        template<typename T>
        struct Utf32CharType
        {
          typedef T Type;
        };

        #define YSTRING_DEFINE_UTF32_CHAR_TYPE(type, internalType) \
            template <> \
            struct Utf32CharType<type> \
            { \
                typedef internalType Type; \
            }

        YSTRING_DEFINE_UTF32_CHAR_TYPE(char, uint8_t);
        YSTRING_DEFINE_UTF32_CHAR_TYPE(int8_t, uint8_t);
        YSTRING_DEFINE_UTF32_CHAR_TYPE(int16_t, char16_t);
        YSTRING_DEFINE_UTF32_CHAR_TYPE(char16_t, char16_t);
        YSTRING_DEFINE_UTF32_CHAR_TYPE(char32_t, char32_t);
        YSTRING_DEFINE_UTF32_CHAR_TYPE(int32_t, char32_t);

        #ifdef YSTRING_WCHAR_IS_2_BYTES
            YSTRING_DEFINE_UTF32_CHAR_TYPE(wchar_t, char16_t);
        #else
            YSTRING_DEFINE_UTF32_CHAR_TYPE(wchar_t, char32_t);
        #endif

        using Utilities::swapEndianness;

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char32_t& word, FwdIt& it, FwdIt end,
                                 uint8_t)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            FwdIt initialIt = it;
            Utilities::Union32 chr;
            chr.u8[SwapBytes ? 3 : 0] = uint8_t(*it++);
            for (auto i = 1; i < 4; ++i)
            {
                if (it == end)
                {
                    it = initialIt;
                    return DecoderResult::INCOMPLETE;
                }
                chr.u8[SwapBytes ? 3 - i : i] = uint8_t(*it++);
            }

            word = chr.u32;
            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char32_t& word, FwdIt& it, FwdIt end,
                                 char32_t)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            word = *it++;
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char32_t& word, FwdIt& it, FwdIt end, char)
        {
            return nextWord<SwapBytes>(word, it, end, uint8_t());
        }

        template <bool SwapBytes, typename FwdIt>
        DecoderResult_t nextWord(char32_t& word, FwdIt& it, FwdIt end,
                                 wchar_t)
        {
            return nextWord<SwapBytes>(word, it, end, char32_t());
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char32_t& word, BiIt begin, BiIt& it,
                                 uint8_t)
        {
            if (it == begin)
                return DecoderResult::END_OF_STRING;

            BiIt initialIt = it;
            Utilities::Union32 chr;
            chr.u8[SwapBytes ? 3 : 0] = uint8_t(*(--it));
            for (auto i = 1; i < 4; ++i)
            {
                if (it == begin)
                {
                    it = initialIt;
                    return DecoderResult::INCOMPLETE;
                }
                chr.u8[SwapBytes ? 3 - i : i] = uint8_t(*(--it));
            }

            word = chr.u32;
            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char32_t& word, BiIt begin, BiIt& it,
                                 char32_t)
        {
            if (it == begin)
                return DecoderResult::END_OF_STRING;

            word = *(--it);
            swapEndianness<SwapBytes>(word);

            return DecoderResult::OK;
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char32_t& word, BiIt begin, BiIt& it, char)
        {
            return prevWord<SwapBytes>(word, begin, it, uint8_t());
        }

        template <bool SwapBytes, typename BiIt>
        DecoderResult_t prevWord(char32_t& word, BiIt begin, BiIt& it,
                                 wchar_t)
        {
            return prevWord<SwapBytes>(word, begin, it, UnsignedWChar());
        }

        template <typename FwdIt, typename T>
        bool skipNextWord(FwdIt& it, FwdIt end, size_t count, T)
        {
            return Utilities::advance(it, end, count);
        }

        template <typename FwdIt>
        bool skipNextWord(FwdIt& it, FwdIt end, size_t count, uint8_t)
        {
            return Utilities::advance(it, end, count * 4);
        }

        template <typename FwdIt>
        bool skipNextWord(FwdIt& it, FwdIt end, size_t count, char)
        {
            return skipNextWord<FwdIt>(it, end, count, uint8_t());
        }

        template <typename BiIt, typename T>
        bool skipPrevWord(BiIt begin, BiIt& it, size_t count, T)
        {
            return Utilities::recede(begin, it, count);
        }

        template <typename BiIt>
        bool skipPrevWord(BiIt begin, BiIt& it, size_t count, uint8_t)
        {
            return Utilities::recede(begin, it, count * 4);
        }

        template <typename BiIt>
        bool skipPrevWord(BiIt begin, BiIt& it, size_t count, char)
        {
            return skipPrevWord<BiIt>(begin, it, count, uint8_t());
        }
    }

    template <bool SwapBytes, typename FwdIt>
    DecoderResult_t nextUtf32CodePoint(char32_t& codePoint, FwdIt& it,
                                       FwdIt end)
    {
        typedef typename std::iterator_traits<FwdIt>::value_type CharType;
        typedef typename Detail::Utf32CharType<CharType>::Type Utf32Type;
        return Detail::nextWord<SwapBytes>(codePoint, it, end, Utf32Type());
    }

    template<bool SwapBytes, typename BiIt>
    DecoderResult_t prevUtf32CodePoint(char32_t& codePoint, BiIt begin,
                                       BiIt& it)
    {
        typedef typename std::iterator_traits<BiIt>::value_type CharType;
        typedef typename Detail::Utf32CharType<CharType>::Type Utf32Type;
        return Detail::prevWord<SwapBytes>(codePoint, begin, it,
                Utf32Type());
    }

    template <typename FwdIt>
    bool skipNextUtf32CodePoint(FwdIt& it, FwdIt end, size_t count)
    {
        typedef typename std::iterator_traits<FwdIt>::value_type CharType;
        typedef typename Detail::Utf32CharType<CharType>::Type Utf32Type;
        return Detail::skipNextWord(it, end, count, Utf32Type());
    }

    template <typename BiIt>
    bool skipPrevUtf32CodePoint(BiIt begin, BiIt& it, size_t count)
    {
        typedef typename std::iterator_traits<BiIt>::value_type CharType;
        typedef typename Detail::Utf32CharType<CharType>::Type Utf32Type;
        return Detail::skipPrevWord(begin, it, count, Utf32Type());
    }
}}
