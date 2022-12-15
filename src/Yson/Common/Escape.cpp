//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-05-31.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Escape.hpp"

#include <algorithm>
#include <iterator>
#include "Yson/YsonException.hpp"

namespace Yson
{
    namespace
    {
        constexpr size_t utf8EncodedLength(char32_t c)
        {
            if (!(c >> 7))
                return 1;
            else if (!(c >> 11))
                return 2;
            else if (!(c >> 16))
                return 3;
            else if (!(c >> 21))
                return 4;
            else if (!(c >> 26))
                return 5;
            else if (!(c >> 31))
                return 6;
            else
                return 7;
        }

        template <typename OutputIt>
        OutputIt addUtf8(OutputIt it, char32_t c, size_t length)
        {
            if (length == 1)
            {
                *it = (char)c;
                ++it;
            }
            else
            {
                auto shift = unsigned((length - 1) * 6);
                *it = (char)((0xFFu << (8 - length)) | (c >> shift));
                ++it;
                for (size_t i = 1; i < length; i++)
                {
                    shift -= 6;
                    *it = (char)(0x80u | ((c >> shift) & 0x3Fu));
                    ++it;
                }
            }
            return it;
        }

        /** @brief Adds @a codePoint encoded as UTF-8 to @a it.
         *
         *  @return the new iterator position.
         */
        template <typename OutputIt>
        OutputIt addUtf8(OutputIt it, char32_t c)
        {
            return addUtf8(it, c, utf8EncodedLength(c));
        }
    }

    namespace
    {
        enum class DecoderResult
        {
            OK = 0,
            END_OF_STRING = 1,
            INCOMPLETE = 2,
            INVALID = 4
        };

        constexpr bool isContinuation(uint8_t c)
        {
            return (c & 0xC0u) == 0x80;
        }

        constexpr bool isAscii(char32_t c)
        {
            return (c & 0x80u) == 0;
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
        DecoderResult nextUtf8CodePoint(char32_t& codePoint,
                                        FwdIt& it, FwdIt end)
        {
            if (it == end)
                return DecoderResult::END_OF_STRING;

            if (isAscii(*it))
            {
                codePoint = (uint8_t)*it++;
                return DecoderResult::OK;
            }

            if (isContinuation(*it) || uint8_t(*it) >= 0xFE)
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
            while (++it != end && count && isContinuation(*it))
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
    }

    namespace
    {
        bool nextCodePoint(char32_t& codePoint,
                           std::string_view::iterator& it,
                           std::string_view::iterator last)
        {
            switch (nextUtf8CodePoint(codePoint, it, last))
            {
            case DecoderResult::END_OF_STRING:
                return false;
            case DecoderResult::INCOMPLETE:
                YSON_THROW("Incomplete character.");
            case DecoderResult::INVALID:
                YSON_THROW("Invalid character.");
            default:
                break;
            }
            return true;
        }

        template <typename UnaryPred>
        bool advanceIfNot(std::string_view::iterator& it,
                          std::string_view::iterator last,
                          UnaryPred pred)
        {
            auto pos = it;
            char32_t ch;
            if (!nextCodePoint(ch, it, last) || pred(ch))
            {
                it = pos;
                return false;
            }
            return true;
        }

        template <typename UnaryPred>
        bool advanceUntil(std::string_view::iterator& it,
                          std::string_view::iterator last,
                          UnaryPred pred)
        {
            while (advanceIfNot(it, last, pred))
            {
            }
            return it != last;
        }
    }

    namespace
    {
        char toCharDigit(uint32_t c)
        {
            return static_cast<char>(c + (c < 0xA ? '0' : 'A' - 10));
        }

        bool isEscapable(char32_t c)
        {
            return c < 32 || c == '"' || c == '\\' || (127 <= c && c < 160);
        }

        bool isEscapableOrNonAscii(char32_t c)
        {
            return c < 32 || c == '"' || c == '\\' || 127 <= c;
        }

        void appendHex(std::string& dst, char32_t value)
        {
            for (auto i = 0; i < 4; ++i)
            {
                auto shift = (3 - i) * 4;
                dst.push_back(toCharDigit((value >> shift) & 0xF));
            }
        }

        void escapeCharacter(std::string& dst, char32_t chr)
        {
            dst.push_back('\\');
            switch (chr)
            {
            case '\b':
                dst.push_back('b');
                break;
            case '\t':
                dst.push_back('t');
                break;
            case '\n':
                dst.push_back('n');
                break;
            case '\f':
                dst.push_back('f');
                break;
            case '\r':
                dst.push_back('r');
                break;
            case '\"':
                dst.push_back('"');
                break;
            case '\\':
                dst.push_back('\\');
                break;
            default:
            {
                if (chr < 0x10000u)
                {
                    dst.push_back('u');
                    for (auto i = 0; i < 4; ++i)
                    {
                        auto shift = (3 - i) * 4;
                        dst.push_back(toCharDigit((chr >> shift) & 0xF));
                    }
                }
                else
                {
                    chr -= 0x10000;
                    auto hi = ((chr >> 10) & 0x3F) | 0xD800;
                    auto lo = (chr & 0x3F) | 0xDC00;
                    dst.push_back('u');
                    appendHex(dst, hi);
                    dst.push_back('\\');
                    dst.push_back('u');
                    appendHex(dst, lo);
                }
            }
                break;
            }
        }

        char32_t fromHex(std::string_view::iterator& first,
                         std::string_view::iterator last)
        {
            auto result = char32_t(0);
            for (size_t i = 0; i < 4; ++i)
            {
                if (first == last)
                    YSON_THROW("Hexadecimal sequence is too short.");
                auto tmp = *first;
                if ('0' <= tmp && tmp <= '9')
                    result = result * 16 + tmp - '0';
                else if ('A' <= tmp && tmp <= 'F')
                    result = result * 16 + tmp - 'A' + 10;
                else if ('a' <= tmp && tmp <= 'f')
                    result = result * 16 + tmp - 'a' + 10;
                else
                    YSON_THROW("Hexadecimal sequence is too short.");
                ++first;
            }
            return result;
        }

        bool unescape(char32_t& result,
                      std::string_view::iterator& first,
                      std::string_view::iterator last)
        {
            if (first == last || *first != '\\')
                return false;
            if (++first == last)
                YSON_THROW("Incomplete ch sequence at end of string.");
            auto ch = *first++;
            switch (ch)
            {
            case 'b':
                result = '\b';
                break;
            case 'f':
                result = '\f';
                break;
            case 'n':
                result = '\n';
                break;
            case 'r':
                result = '\r';
                break;
            case 't':
                result = '\t';
                break;
            case 'u':
                result = fromHex(first, last);
                break;
            default:
                result = uint8_t(ch);
                break;
            }
            return true;
        }
    }

    std::string escape(std::string_view str,
                       bool escapeNonAscii)
    {
        std::string result;
        auto first = begin(str);
        auto it = begin(str);
        auto func = escapeNonAscii ? isEscapableOrNonAscii : isEscapable;
        while (advanceUntil(it, end(str), func))
        {
            result.append(first, it);
            char32_t c;
            nextCodePoint(c, it, end(str));
            escapeCharacter(result, c);
            first = it;
        }
        result.append(first, it);
        return result;
    }

    bool hasEscapedCharacters(std::string_view str)
    {
        return end(str) != std::find(begin(str), end(str), '\\');
    }

    bool hasUnescapedCharacters(std::string_view str,
                                bool escapeNonAscii)
    {
        auto it = begin(str);
        if (escapeNonAscii)
            return advanceUntil(it, end(str), isEscapableOrNonAscii);
        else
            return advanceUntil(it, end(str), isEscapable);
    }

    std::string unescape(std::string_view str)
    {
        std::string result;
        auto first = str.begin();
        auto last = std::find(first, str.end(), '\\');
        while (last != str.end())
        {
            result.append(first, last);
            char32_t ch;
            first = last;
            if (unescape(ch, first, str.end()))
            {
                if ((ch & 0xFC00) != 0xD800
                    || first == str.end() || *first != '\\')
                {
                    addUtf8(back_inserter(result), ch);
                }
                else
                {
                    char32_t ch2;
                    if (unescape(ch2, first, str.end())
                        && (ch2 & 0xFC00) == 0xDC00)
                    {
                        ch -= 0xD800;
                        ch *= 0x400;
                        ch += ch2 - 0xDC00 + 0x10000;
                        addUtf8(back_inserter(result), ch);
                    }
                    else
                    {
                        addUtf8(back_inserter(result), ch);
                        addUtf8(back_inserter(result), ch2);
                    }
                }
            }
            last = std::find(first, str.end(), '\\');
        }
        result.append(first, str.end());
        return result;
    }
}
