//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-05-31.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Escape.hpp"

#include "../Encodings/EncodeUtf8.hpp"
#include "DecoderAlgorithms.hpp"

namespace Ystring { namespace Utf8
{
    namespace
    {
        void escapeCharacter(std::string& dst, char32_t chr);

        bool isEscapable(char32_t c);

        bool unescape(char32_t& result,
                      std::string::const_iterator& first,
                      std::string::const_iterator last);
    }

    std::string escape(const std::string& str)
    {
        std::string result;
        auto first = begin(str);
        auto it = begin(str);
        while (advanceUntil(it, end(str), isEscapable))
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

    bool hasUnescapedCharacters(const std::string& str)
    {
        auto it = begin(str);
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
                Encodings::addUtf8(back_inserter(result), ch);
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
                if (chr > 0xFFFFu)
                {
                    YSTRING_THROW("Character " + std::to_string(int64_t(chr))
                                  + " has more than 4 hex digits.");
                }
                dst.push_back('u');
                for (auto i = 0; i < 4; ++i)
                {
                    auto shift = (3 - i) * 4;
                    dst.push_back(toCharDigit((chr >> shift) & 0xF));
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
}}
