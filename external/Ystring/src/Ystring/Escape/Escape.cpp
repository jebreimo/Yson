//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-05-31.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Ystring/Escape/Escape.hpp"

#include "Ystring/Encodings/EncodeUtf8.hpp"
#include "DecoderAlgorithms.hpp"

namespace Ystring
{
    namespace
    {
        void escapeCharacter(std::string& dst, char32_t chr);

        bool isEscapable(char32_t c);

        bool isEscapableOrNonAscii(char32_t c);

        bool unescape(char32_t& result,
                      std::string::const_iterator& first,
                      std::string::const_iterator last);
    }

    std::string escape(const std::string& str,
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

    bool hasEscapedCharacters(const std::string& str)
    {
        return end(str) != std::find(begin(str), end(str), '\\');
    }

    bool hasUnescapedCharacters(const std::string& str,
                                bool escapeNonAscii)
    {
        auto it = begin(str);
        if (escapeNonAscii)
            return advanceUntil(it, end(str), isEscapableOrNonAscii);
        else
            return advanceUntil(it, end(str), isEscapable);
    }

    std::string unescape(const std::string& str)
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
                    Encodings::addUtf8(back_inserter(result), ch);
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
                        Encodings::addUtf8(back_inserter(result), ch);
                    }
                    else
                    {
                        Encodings::addUtf8(back_inserter(result), ch);
                        Encodings::addUtf8(back_inserter(result), ch2);
                    }
                }
            }
            last = std::find(first, str.end(), '\\');
        }
        result.append(first, str.end());
        return result;
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
            case '\b': dst.push_back('b'); break;
            case '\t': dst.push_back('t'); break;
            case '\n': dst.push_back('n'); break;
            case '\f': dst.push_back('f'); break;
            case '\r': dst.push_back('r'); break;
            case '\"': dst.push_back('"'); break;
            case '\\': dst.push_back('\\'); break;
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

        char32_t fromHex(std::string::const_iterator& first,
                         std::string::const_iterator last)
        {
            auto result = char32_t(0);
            for (size_t i = 0; i < 4; ++i)
            {
                if (first == last)
                    YSTRING_THROW("Hexadecimal sequence is too short.");
                auto tmp = *first;
                if ('0' <= tmp && tmp <= '9')
                    result = result * 16 + tmp - '0';
                else if ('A' <= tmp && tmp <= 'F')
                    result = result * 16 + tmp - 'A' + 10;
                else if ('a' <= tmp && tmp <= 'f')
                    result = result * 16 + tmp - 'a' + 10;
                else if (i < 4)
                    YSTRING_THROW("Hexadecimal sequence is too short.");
                else
                    break;
                ++first;
            }
            return result;
        }

        bool unescape(char32_t& result,
                      std::string::const_iterator& first,
                      std::string::const_iterator last)
        {
            if (first == last || *first != '\\')
                return false;
            if (++first == last)
                YSTRING_THROW("Incomplete escape sequence at end of string.");
            auto escape = *first++;
            switch (escape)
            {
            case 'b': result = '\b'; break;
            case 'f': result = '\f'; break;
            case 'n': result = '\n'; break;
            case 'r': result = '\r'; break;
            case 't': result = '\t'; break;
            case 'u': result = fromHex(first, last); break;
            default: result = uint8_t(escape); break;
            }
            return true;
        }
    }
}
