//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Encoding.hpp"

#include <algorithm>
#include <istream>

namespace Yconvert
{
    namespace
    {
        constexpr EncodingInfo ENCODING_INFO[] =
            {
                {Encoding::UNKNOWN, "<unknown>", "",
                 0, 0, Endianness::UNKNOWN},
                {Encoding::UTF_8, "UTF-8", "\xEF\xBB\xBF",
                 1, 4, Endianness::UNKNOWN},
                {Encoding::UTF_16_LE, "UTF-16LE", "\xFF\xFE",
                 2, 2, Endianness::BIG},
                {Encoding::UTF_16_BE, "UTF-16BE", "\xFE\xFF",
                 2, 2, Endianness::BIG},
                {Encoding::UTF_32_LE, "UTF-32LE", {"\xFF\xFE\x00\x00", 4},
                 4, 1, Endianness::BIG},
                {Encoding::UTF_32_BE, "UTF-32BE", {"\x00\x00\xFE\xFF", 4},
                 4, 1, Endianness::BIG},
                {Encoding::ASCII, "ASCII", "",
                 1, 1, Endianness::UNKNOWN},
            #ifdef YCONVERT_ENABLE_ISO_CODE_PAGES
                {Encoding::ISO_8859_1, "ISO-8859-1", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_2, "ISO-8859-2", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_3, "ISO-8859-3", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_4, "ISO-8859-4", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_5, "ISO-8859-5", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_6, "ISO-8859-6", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_7, "ISO-8859-7", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_8, "ISO-8859-8", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_9, "ISO-8859-9", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_10, "ISO-8859-10", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_11, "ISO-8859-11", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_13, "ISO-8859-13", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_14, "ISO-8859-14", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_15, "ISO-8859-15", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::ISO_8859_16, "ISO-8859-16", "",
                 1, 1, Endianness::UNKNOWN},
            #endif
            #ifdef YCONVERT_ENABLE_MAC_CODE_PAGES
                {Encoding::MAC_CYRILLIC, "MAC-CYRILLIC", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::MAC_GREEK, "MAC-GREEK", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::MAC_ICELAND, "MAC-ICELAND", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::MAC_LATIN2, "MAC-LATIN2", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::MAC_ROMAN, "MAC-ROMAN", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::MAC_TURKISH, "MAC-TURKISH", "",
                 1, 1, Endianness::UNKNOWN},
            #endif
            #ifdef YCONVERT_ENABLE_DOS_CODE_PAGES
                {Encoding::DOS_CP437, "DOS-CP437", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP737, "DOS-CP737", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP775, "DOS-CP775", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP850, "DOS-CP850", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP852, "DOS-CP852", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP855, "DOS-CP855", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP857, "DOS-CP857", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP860, "DOS-CP860", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP861, "DOS-CP861", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP862, "DOS-CP862", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP863, "DOS-CP863", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP864, "DOS-CP864", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP865, "DOS-CP865", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP866, "DOS-CP866", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP869, "DOS-CP869", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::DOS_CP874, "DOS-CP874", "",
                 1, 1, Endianness::UNKNOWN},
            #endif
            #ifdef YCONVERT_ENABLE_WIN_CODE_PAGES
                {Encoding::WIN_CP1250, "WIN-CP1250", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1251, "WIN-CP1251", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1252, "WIN-CP1252", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1253, "WIN-CP1253", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1254, "WIN-CP1254", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1255, "WIN-CP1255", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1256, "WIN-CP1256", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1257, "WIN-CP1257", "",
                 1, 1, Endianness::UNKNOWN},
                {Encoding::WIN_CP1258, "WIN-CP1258", "",
                 1, 1, Endianness::UNKNOWN},
            #endif
            };

        typedef std::pair<std::string, Encoding> EncodingName;

        static EncodingName ENCODING_ALIASES[] = {
            EncodingName("UTF-16", Encoding::UTF_16_NATIVE),
            EncodingName("UTF-32", Encoding::UTF_32_NATIVE),
            #ifdef YCONVERT_ENABLE_WIN_CODE_PAGES
            EncodingName("WINDOWS-1250", Encoding::WIN_CP1250),
            EncodingName("WINDOWS-1251", Encoding::WIN_CP1251),
            EncodingName("WINDOWS-1252", Encoding::WIN_CP1252),
            EncodingName("WINDOWS-1253", Encoding::WIN_CP1253),
            EncodingName("WINDOWS-1254", Encoding::WIN_CP1254),
            EncodingName("WINDOWS-1255", Encoding::WIN_CP1255),
            EncodingName("WINDOWS-1256", Encoding::WIN_CP1256),
            EncodingName("WINDOWS-1257", Encoding::WIN_CP1257),
            EncodingName("WINDOWS-1258", Encoding::WIN_CP1258),
            #endif
            #ifdef YCONVERT_ENABLE_ISO_CODE_PAGES
            EncodingName("LATIN1", Encoding::ISO_8859_1),
            EncodingName("LATIN2", Encoding::ISO_8859_2),
            EncodingName("LATIN3", Encoding::ISO_8859_3),
            EncodingName("LATIN4", Encoding::ISO_8859_4),
            EncodingName("LATIN5", Encoding::ISO_8859_9),
            EncodingName("LATIN6", Encoding::ISO_8859_10),
            EncodingName("LATIN7", Encoding::ISO_8859_13),
            EncodingName("LATIN8", Encoding::ISO_8859_14),
            EncodingName("LATIN9", Encoding::ISO_8859_15),
            EncodingName("LATIN10", Encoding::ISO_8859_16),
            #endif
        };
    }

    const EncodingInfo& getEncodingInfo(Encoding encoding)
    {
        using std::begin;
        using std::end;
        auto match = std::lower_bound(
            begin(ENCODING_INFO), end(ENCODING_INFO),
            EncodingInfo{encoding},
            [](auto& a, auto& b){return a.encoding < b.encoding;});
        if (match != end(ENCODING_INFO))
            return *match;
        return ENCODING_INFO[0];
    }

    std::pair<const EncodingInfo*, size_t> getSupportedEncodings()
    {
        return {&ENCODING_INFO[1], std::size(ENCODING_INFO) - 1};
    }


    Encoding encodingFromName(std::string name)
    {
        std::transform(name.begin(), name.end(), name.begin(), [](auto c)
        {
            return 'a' <= c && c <= 'z' ? char(c - 32) : c;
        });
        using std::begin;
        using std::end;
        auto it = std::find_if(begin(ENCODING_INFO), end(ENCODING_INFO),
                               [&](auto& i){return i.name == name;});
        if (it != end(ENCODING_INFO))
            return it->encoding;
        auto jt = std::find_if(begin(ENCODING_ALIASES), end(ENCODING_ALIASES),
                               [&](auto& i) {return i.first == name;});
        if (jt != end(ENCODING_ALIASES))
            return jt->second;
        return Encoding::UNKNOWN;
    }

    Encoding determineEncodingFromByteOrderMark(const char* str, size_t len)
    {
        auto size = sizeof(ENCODING_INFO) / sizeof(*ENCODING_INFO);
        Encoding encoding = Encoding::UNKNOWN;
        size_t bomLength = 0;
        for (size_t i = 0; i < size; i++)
        {
            auto& bom = ENCODING_INFO[i].byteOrderMark;
            if (!bom.empty()
                && len >= bom.size()
                && bom.size() > bomLength
                && std::equal(bom.begin(), bom.end(), str))
            {
                encoding = ENCODING_INFO[i].encoding;
                bomLength = bom.size();
            }
        }
        return encoding;
    }

    unsigned getUtf8CharLength(uint8_t chr)
    {
        if (!(chr & 0x80u))
            return 1;
        if ((chr & 0xE0u) == 0xC0u)
            return 2;
        if ((chr & 0xF0u) == 0xE0u)
            return 3;
        if ((chr & 0xF8u) == 0xF0u)
            return 4;
        return 0;
    }

    bool isValidUtf8Char(const char* str, size_t len)
    {
        if (len == 0)
            return false;
        auto n = getUtf8CharLength(str[0]);
        if (n == 0 || n > len)
            return false;
        for (unsigned i = 1; i < n; ++i)
        {
            if ((uint8_t(str[i]) & 0xC0u) != 0x80u)
                return false;
        }
        return true;
    }

    Encoding determineEncodingFromFirstCharacter(const char* str, size_t len)
    {
        if (len == 0)
            return Encoding::UNKNOWN;

        len = std::min(len, size_t(4));
        unsigned nonZeroPattern = 0;
        unsigned nonAsciiPattern = 0;
        for (size_t i = 0; i < len; ++i)
        {
            nonAsciiPattern <<= 1u;
            nonAsciiPattern |= uint8_t(str[i]) > 127 ? 1u : 0u;

            nonZeroPattern <<= 1u;
            nonZeroPattern |= str[i] != 0 ? 1u : 0u;
        }

        if ((1u << len) - 1 == nonZeroPattern)
        {
            if (nonAsciiPattern == 0 || isValidUtf8Char(str, len))
                return Encoding::UTF_8;
            return Encoding::UNKNOWN;
        }

        if (len == 4)
        {
            switch (nonZeroPattern)
            {
            case 0b0001:
            case 0b0010:
            case 0b0011:
                return Encoding::UTF_32_BE;
            case 0b0100:
            case 0b1000:
            case 0b1100:
                return Encoding::UTF_32_LE;
            case 0b0101:
                return Encoding::UTF_16_BE;
            case 0b1010:
                return Encoding::UTF_16_LE;
            default:
                nonZeroPattern >>= 2u;
                len = 2;
                break;
            }
        }
        if (len == 2)
        {
            if (nonZeroPattern == 0b10)
                return Encoding::UTF_16_LE;
            else if (nonZeroPattern == 0b01)
                return Encoding::UTF_16_BE;
        }

        return Encoding::UNKNOWN;
    }

    std::pair<Encoding, size_t> determineEncoding(const char* buffer, size_t length)
    {
        auto enc = determineEncodingFromByteOrderMark(buffer, length);
        if (enc != Encoding::UNKNOWN)
            return {enc, getEncodingInfo(enc).byteOrderMark.size()};

        enc = determineEncodingFromFirstCharacter(buffer, length);
        return {enc, 0};
    }

    Encoding determineEncoding(std::istream& stream)
    {
        auto startPos = stream.tellg();
        char buf[4];
        stream.read(buf, 4);
        auto [enc, offset] = determineEncoding(buf, stream.gcount());
        if (offset != 4)
            stream.seekg(std::streamsize(startPos) + offset, std::ios::beg);
        return enc;
    }
}
