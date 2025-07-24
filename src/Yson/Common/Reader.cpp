//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-09-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <algorithm>
#include <fstream>
#include "Yconvert/Encoding.hpp"
#include "Yson/JsonReader.hpp"
#include "Yson/UBJsonReader.hpp"
#include "Yson/YsonException.hpp"

namespace Yson
{
    namespace
    {
        enum class ContentType
        {
            UNKNOWN,
            JSON,
            UBJSON
        };

        constexpr char JSON_CHARACTERS[] = "\0\t\r\n \"/0123456789fnt";
        constexpr char UBJSON_CHARACTERS[] = "#$CDFHILNSTUZdil";

        /**
         * @brief Checks if @a contents appears to start with a BOM.
         *
         * This is not a rock solid test, but then it doesn't have to be
         * either as the parser will handle
         * @param contents A buffer.
         * @param size The size of contents.
         * @return true if @a contents starts with a BOM.
         */
        bool isByteOrderMark(const char* contents, size_t size)
        {
            const auto encInf = Yson_Yconvert::determine_encoding_from_byte_order_mark(
                    contents, size);
            switch (encInf)
            {
            case Yson_Yconvert::Encoding::UTF_8:
            case Yson_Yconvert::Encoding::UTF_16_BE:
            case Yson_Yconvert::Encoding::UTF_16_LE:
            case Yson_Yconvert::Encoding::UTF_32_BE:
            case Yson_Yconvert::Encoding::UTF_32_LE:
                return true;
            default:
                return false;
            }
        }

        ContentType identifyFile(const char* contents, size_t size)
        {
            if (size == 0 || isByteOrderMark(contents, size))
                return ContentType::JSON;
            bool allowComma = false;
            std::vector<char> scopes;
            for (size_t i = 0; i < size; ++i)
            {
                switch (contents[i])
                {
                case '{':
                    scopes.push_back('}');
                    allowComma = false;
                    break;
                case '[':
                    scopes.push_back(']');
                    allowComma = false;
                    break;
                case '}':
                case ']':
                    if (scopes.empty() || scopes.back() != contents[i])
                        return ContentType::UNKNOWN;
                    scopes.pop_back();
                    allowComma = !scopes.empty();
                case ',':
                    return allowComma ? ContentType::JSON : ContentType::UNKNOWN;
                default:
                    if (std::find(std::begin(JSON_CHARACTERS),
                                  std::end(JSON_CHARACTERS),
                                  contents[i]) != std::end(JSON_CHARACTERS))
                        return ContentType::JSON;
                    if (std::find(std::begin(UBJSON_CHARACTERS),
                                  std::end(UBJSON_CHARACTERS),
                                  contents[i]) != std::end(UBJSON_CHARACTERS))
                        return ContentType::UBJSON;
                    return ContentType::UNKNOWN;
                }
            }
            return ContentType::UNKNOWN;
        }
    }

    std::unique_ptr<Reader> makeReader(std::istream& stream)
    {
        std::vector<char> buffer(1024);
        stream.read(buffer.data(),
                    static_cast<std::streamsize>(buffer.size()));
        buffer.resize(stream.gcount());
        const auto contentType = identifyFile(buffer.data(), buffer.size());
        if (contentType == ContentType::JSON)
            return std::make_unique<JsonReader>(stream, buffer.data(),
                                                buffer.size());
        if (contentType == ContentType::UBJSON)
            return std::make_unique<UBJsonReader>(stream, buffer.data(),
                                                  buffer.size());

        YSON_THROW("Stream contents appear to be neither JSON nor UBJSON.");
    }

    std::unique_ptr<Reader> makeReader(const std::filesystem::path& fileName)
    {
        std::ifstream file(fileName, std::ios_base::binary);
        if (!file)
            YSON_THROW("File not found: " + fileName.string());
        std::vector<char> buffer(1024);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        buffer.resize(file.gcount());
        file.close();
        const auto contentType = identifyFile(buffer.data(), buffer.size());
        if (contentType == ContentType::JSON)
            return std::make_unique<JsonReader>(fileName);

        if (contentType == ContentType::UBJSON)
            return std::make_unique<UBJsonReader>(fileName);

        YSON_THROW("File contents appear to be neither JSON nor UBJSON.");
    }

    std::unique_ptr<Reader> makeReader(const char* buffer, size_t bufferSize)
    {
        const auto contentType = identifyFile(buffer, bufferSize);
        if (contentType == ContentType::JSON)
            return std::make_unique<JsonReader>(buffer, bufferSize);

        if (contentType == ContentType::UBJSON)
            return std::make_unique<UBJsonReader>(buffer, bufferSize);

        YSON_THROW("Buffer contents appear to be neither JSON nor UBJSON.");
    }
}
