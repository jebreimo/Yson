//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include "Writer.hpp"

namespace Yson
{
    class YSON_API JsonWriter : public Writer
    {
    public:
        /**
         * @brief Creates a JSON writer that writes to an internal buffer.
         *
         * The buffer can be retrieved with the buffer() function.
         *
         * @param formatting the automatic formatting that will be used.
         */
        explicit JsonWriter(JsonFormatting formatting = JsonFormatting::FORMAT);

        /**
         * @brief Creates a JSON writer that creates and writes to a file
         *      named @a fileName.
         * @param fileName The UTF-8 encoded name of the file.
         * @param formatting the automatic formatting that will be used.
         */
        explicit JsonWriter(const std::string& fileName,
                            JsonFormatting formatting = JsonFormatting::FORMAT);

        explicit JsonWriter(std::ostream& stream,
                            JsonFormatting formatting = JsonFormatting::FORMAT);

        JsonWriter(const JsonWriter&) = delete;

        JsonWriter(JsonWriter&& rhs) noexcept;

        ~JsonWriter() override;

        JsonWriter& operator=(const JsonWriter&) = delete;

        JsonWriter& operator=(JsonWriter&& rhs) noexcept;

        std::ostream* stream() override;

        [[nodiscard]]
        std::pair<const void*, size_t> buffer() const override;

        [[nodiscard]]
        const std::string& key() const override;

        JsonWriter& key(std::string key) override;

        JsonWriter& beginArray() override;

        JsonWriter& beginArray(
                const StructureParameters& parameters) override;

        JsonWriter& endArray() override;

        JsonWriter& beginObject() override;

        JsonWriter& beginObject(
                const StructureParameters& parameters) override;

        JsonWriter& endObject() override;

        JsonWriter& null() override;

        JsonWriter& boolean(bool value) override;

        JsonWriter& value(char value) override;

        JsonWriter& value(signed char value) override;

        JsonWriter& value(short value) override;

        JsonWriter& value(int value) override;

        JsonWriter& value(long value) override;

        JsonWriter& value(long long value) override;

        JsonWriter& value(unsigned char value) override;

        JsonWriter& value(unsigned short value) override;

        JsonWriter& value(unsigned value) override;

        JsonWriter& value(unsigned long value) override;

        JsonWriter& value(unsigned long long value) override;

        JsonWriter& value(float value) override;

        JsonWriter& value(double value) override;

        JsonWriter& value(long double value);

        JsonWriter& value(std::wstring_view value) override;

        JsonWriter& value(std::string_view value) override;

        JsonWriter& binary(const void* data, size_t size) override;

        JsonWriter& base64(const void* data, size_t size) override;

        JsonWriter& rawValue(std::string_view value);

        JsonWriter& rawText(std::string_view value);

        JsonWriter& indent();

        JsonWriter& outdent();

        JsonWriter& writeComma();

        JsonWriter& writeIndentation();

        JsonWriter& writeNewline();

        JsonWriter& writeSeparator(size_t count = 1);

        [[nodiscard]]
        std::pair<char, unsigned> indentation() const;

        JsonWriter& setIndentation(char character, unsigned width);

        [[nodiscard]]
        bool isFormattingEnabled() const;

        JsonWriter& setFormattingEnabled(bool value);

        [[nodiscard]]
        int languageExtensions() const;

        JsonWriter& setLanguageExtensions(int value);

        /** @brief Returns true if special floating point values will be
          *     written as values.
          *
          * When this flag is enabled, the special values not-a-number and
          * positive and negative infinity are written as "NaN", "Infinity"
          * and "-Infinity" respectively. If it is disabled, the write
          * functions will throw an exception if it encounters any of these
          * values.
          */
        [[nodiscard]]
        bool isNonFiniteFloatsEnabled() const;

        JsonWriter& setNonFiniteFloatsEnabled(bool value);

        [[nodiscard]]
        bool isUnquotedValueNamesEnabled() const;

        JsonWriter& setUnquotedValueNamesEnabled(bool value);

        [[nodiscard]]
        bool isEscapeNonAsciiCharactersEnabled() const;

        JsonWriter& setEscapeNonAsciiCharactersEnabled(bool value);

        [[nodiscard]]
        bool isMultilineStringsEnabled() const;

        JsonWriter& setMultilineStringsEnabled(bool value);

        [[nodiscard]]
        int maximumLineWidth() const;

        JsonWriter& setMaximumLineWidth(int value);

        [[nodiscard]]
        int floatingPointPrecision() const;

        JsonWriter& setFloatingPointPrecision(int value);

        JsonWriter& flush() override;
    private:
        struct Members;

        [[nodiscard]]
        Members& members() const;

        void write(std::string_view s);

        void write(const char* s, size_t size);

        void writeMultiLine(std::string_view s);

        JsonWriter& writeString(std::string_view text);

        JsonWriter(std::unique_ptr<std::ostream> streamPtr,
                   std::ostream* stream,
                   JsonFormatting formatting = JsonFormatting::NONE);

        void beginValue();

        [[nodiscard]]
        JsonFormatting formatting() const;

        [[nodiscard]]
        bool isInsideObject() const;

        JsonWriter& beginStructure(char startChar, char endChar,
                                   JsonParameters parameters = {});

        JsonWriter& endStructure(char endChar);

        void writeIndentationImpl();

        template <typename T>
        JsonWriter& writeIntValueImpl(T number, const char* format);

        template <typename T>
        JsonWriter& writeFloatValueImpl(T number, const char* format);

        enum LanguageExtensions
        {
            NON_FINITE_FLOATS = 1,
            MULTILINE_STRINGS = 2,
            UNQUOTED_VALUE_NAMES = 4,
            ESCAPE_NON_ASCII_CHARACTERS = 8
        };

        [[nodiscard]]
        bool languageExtension(LanguageExtensions ext) const;

        JsonWriter& setLanguageExtension(LanguageExtensions ext, bool value);

        std::unique_ptr<Members> m_Members;
    };
}
