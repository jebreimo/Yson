//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 29.05.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Base64.hpp"
#include "../../Ystring/Utilities/Union32.hpp"

namespace Yson
{
    const char EncodingTable[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "+/";

    const uint8_t Err = 0xFF;
    const uint8_t DecodingTable[128] = {
            Err, Err, Err, Err, Err, Err, Err, Err, // 0x00
            Err, Err, Err, Err, Err, Err, Err, Err, // 0x08
            Err, Err, Err, Err, Err, Err, Err, Err, // 0x10
            Err, Err, Err, Err, Err, Err, Err, Err, // 0x18
            Err, Err, Err, Err, Err, Err, Err, Err, // 0x20
            Err, Err, Err,  62, Err, Err, Err,  63, // 0x28
             52,  53,  54,  55,  56,  57,  58,  59, // 0x30
             60,  61, Err, Err, Err, Err, Err, Err, // 0x38
            Err,   0,   1,   2,   3,   4,   5,   6, // 0x40
              7,   8,   9,  10,  11,  12,  13,  14, // 0x48
             15,  16,  17,  18,  19,  20,  21,  22, // 0x50
             23,  24,  25, Err, Err, Err, Err, Err, // 0x58
            Err,  26,  27,  28,  29,  30,  31,  32, // 0x60
             33,  34,  35,  36,  37,  38,  39,  40, // 0x68
             41,  42,  43,  44,  45,  46,  47,  48, // 0x70
             49,  50,  51, Err, Err, Err, Err, Err, // 0x78
    };

    constexpr size_t getEncodedSize(size_t decodedSize)
    {
        return 4 * (decodedSize + 2 / 3);
    }

    constexpr size_t getDecodedSize(size_t encodedSize)
    {
        return (3 * encodedSize) / 4;
    }

    uint8_t decodeCharacter(char c)
    {
        if (c < 0 || DecodingTable[unsigned(c)] == Err)
            throw std::logic_error(std::string("Invalid Base64 character: '") + c + "'");
        return DecodingTable[unsigned(c)];
    }

    std::string toBase64(const void* data, size_t size)
    {
        std::string result;
        result.reserve(getEncodedSize(size));
        auto data8 = static_cast<const uint8_t*>(data);
        auto tailSize = size % 3;
        auto mainSize = size - tailSize;
        size_t i = 0;
        for (; i < mainSize; i += 3)
        {
            uint32_t word = (data8[i] << 16) | (data8[i + 1] << 8)
                            | data8[i + 2];
            result.push_back(EncodingTable[(word >> 18) & 0x3F]);
            result.push_back(EncodingTable[(word >> 12) & 0x3F]);
            result.push_back(EncodingTable[(word >> 6) & 0x3F]);
            result.push_back(EncodingTable[word & 0x3F]);
        }

        if (tailSize == 1)
        {
            uint32_t word = (data8[i] << 16);
            result.push_back(EncodingTable[(word >> 18) & 0x3F]);
            result.push_back(EncodingTable[(word >> 12) & 0x3F]);
            result.push_back('=');
            result.push_back('=');
        }
        else if (tailSize == 2)
        {
            uint32_t word = (data8[i] << 16) | (data8[i + 1] << 8);
            result.push_back(EncodingTable[(word >> 18) & 0x3F]);
            result.push_back(EncodingTable[(word >> 12) & 0x3F]);
            result.push_back(EncodingTable[(word >> 6) & 0x3F]);
            result.push_back('=');
        }
        return result;
    }

    std::vector<char> fromBase64(const std::string& text)
    {
        std::vector<char> result;
        auto last = text.find_last_not_of('=');
        if (last == std::string::npos)
            return result;
        auto size = last + 1;
        result.reserve(getDecodedSize(size));
        auto tailSize = size % 4;
        auto mainSize = size - tailSize;
        size_t i = 0;
        for (; i < mainSize; i += 4)
        {
            uint32_t word = (decodeCharacter(text[i]) << 18)
                            | (decodeCharacter(text[i + 1]) << 12)
                            | (decodeCharacter(text[i + 2]) << 6)
                            | decodeCharacter(text[i + 3]);
            result.push_back(char(word >> 16));
            result.push_back(char(word >> 8));
            result.push_back(char(word));
        }
        if (tailSize == 3)
        {
            uint32_t word = (decodeCharacter(text[i]) << 18)
                            | (decodeCharacter(text[i + 1]) << 12)
                            | (decodeCharacter(text[i + 2]) << 6);
            result.push_back(char(word >> 16));
            result.push_back(char(word >> 8));
        }
        else if (tailSize == 2)
        {
            uint32_t word = (decodeCharacter(text[i]) << 18)
                            | (decodeCharacter(text[i + 1]) << 12);
            result.push_back(uint8_t(word >> 16));
        }
        return result;
    }

}
