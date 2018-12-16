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
#include "YsonDefinitions.hpp"

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
        JsonWriter(JsonFormatting formatting = JsonFormatting::NONE);

        /**
         * @brief Creates a JSON writer that creates and writes to a file
         *      named @a fileName.
         * @param fileName The UTF-8 encoded name of the file.
         * @param formatting the automatic formatting that will be used.
         */
        explicit JsonWriter(const std::string& fileName,
                            JsonFormatting formatting = JsonFormatting::NONE);

        JsonWriter(std::ostream& stream,
                   JsonFormatting formatting = JsonFormatting::NONE);

        JsonWriter(const JsonWriter&) = delete;

        JsonWriter(JsonWriter&& rhs) noexcept;

        ~JsonWriter();

        JsonWriter& operator=(const JsonWriter&) = delete;

        JsonWriter& operator=(JsonWriter&& rhs) noexcept;

        std::ostream* stream() override;

        std::pair<const void*, size_t> buffer() const override;

        const std::string& key() const override;

        JsonWriter& key(const std::string& key) override;

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

        JsonWriter& value(int8_t value) override;

        JsonWriter& value(int16_t value) override;

        JsonWriter& value(int32_t value) override;

        JsonWriter& value(int64_t value) override;

        JsonWriter& value(uint8_t value) override;

        JsonWriter& value(uint16_t value) override;

        JsonWriter& value(uint32_t value) override;

        JsonWriter& value(uint64_t value) override;

        JsonWriter& value(float value) override;

        JsonWriter& value(double value) override;

        JsonWriter& value(long double value);

        JsonWriter& value(const std::string& text) override;

        JsonWriter& value(const std::wstring& text) override;

        JsonWriter& binary(const void* data, size_t size) override;

        JsonWriter& base64(const void* data, size_t size) override;

        JsonWriter& rawValue(const std::string& value);

        JsonWriter& rawText(const std::string& value);

        JsonWriter& indent();

        JsonWriter& outdent();

        JsonWriter& writeComma();

        JsonWriter& writeIndentation();

        JsonWriter& writeNewline();

        JsonWriter& writeSeparator(size_t count = 1);

        std::pair<char, unsigned> indentation() const;

        JsonWriter& setIndentation(char character, unsigned width);

        bool isFormattingEnabled() const;

        JsonWriter& setFormattingEnabled(bool value);

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
        bool isNonFiniteFloatsEnabled() const;

        JsonWriter& setNonFiniteFloatsEnabled(bool value);

        bool isUnquotedValueNamesEnabled() const;

        JsonWriter& setUnquotedValueNamesEnabled(bool value);

        bool isMultilineStringsEnabled() const;

        JsonWriter& setMultilineStringsEnabled(bool value);

        int maximumLineWidth() const;

        JsonWriter& setMaximumLineWidth(int value);

        int floatingPointPrecision() const;

        JsonWriter& setFloatingPointPrecision(int value);

        JsonWriter& flush() override;
    private:
        struct Members;

        Members& members() const;

        void write(const std::string& s);

        void write(const char* s, size_t size);

        void writeMultiLine(const std::string& s);

        JsonWriter(std::unique_ptr<std::ostream> streamPtr,
                   std::ostream* stream,
                   JsonFormatting formatting = JsonFormatting::NONE);

        void beginValue();

        JsonFormatting formatting() const;

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
            UNQUOTED_VALUE_NAMES = 4
        };

        bool languageExtension(LanguageExtensions ext) const;

        JsonWriter& setLanguageExtension(LanguageExtensions ext, bool value);

        std::unique_ptr<Members> m_Members;
    };
}
