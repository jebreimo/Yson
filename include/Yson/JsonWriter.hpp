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
    /**
     * Writer class for JSON data.
     */
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

        /**
         * @brief Creates a JSON writer that writes to the specified stream.
         *
         * @param stream The stream to write to.
         * @param formatting the automatic formatting that will be used.
         */
        explicit JsonWriter(std::ostream& stream,
                            JsonFormatting formatting = JsonFormatting::FORMAT);

        ~JsonWriter() override;

        JsonWriter(const JsonWriter&) = delete;

        JsonWriter(JsonWriter&& rhs) noexcept;

        JsonWriter& operator=(const JsonWriter&) = delete;

        JsonWriter& operator=(JsonWriter&& rhs) noexcept;

        /**
         * @name Functions for writing objects and arrays
         */
        ///@{

        /**
         * @brief Returns the key that will be used for the next value in the
         *  current object.
         */
        [[nodiscard]]
        const std::string& key() const override;

        /**
         * @brief Sets the key of the next value.
         *
         * If the current context is not an object then this function has
         * no effect, the same is true if the call to this function is
         * not followed by a call to one of the value functions.
         */
        JsonWriter& key(std::string key) override;

        /**
         * @brief Begins an array.
         */
        JsonWriter& beginArray() override;

        /**
         * @brief Begins an array with the specified parameters.
         */
        JsonWriter& beginArray(
                const StructureParameters& parameters) override;

        /**
         * Ends the current array.
         */
        JsonWriter& endArray() override;

        /**
         * Begins an object.
         */
        JsonWriter& beginObject() override;

        /**
         * Begins an object with the specified parameters.
         */
        JsonWriter& beginObject(
                const StructureParameters& parameters) override;

        /**
         * Ends the current object.
         */
        JsonWriter& endObject() override;

        ///@}

        /**
         * @name Functions for writing values
         */
        ///@{

        /**
         * @brief Writes a null value.
         */
        JsonWriter& null() override;

        /**
         * @brief Writes a boolean value.
         */
        JsonWriter& boolean(bool value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(char value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(signed char value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(short value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(int value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(long value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(long long value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(unsigned char value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(unsigned short value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(unsigned value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(unsigned long value) override;

        /**
         * @brief Writes an integer value.
         */
        JsonWriter& value(unsigned long long value) override;

        /**
         * @brief Writes a 32-bit floating point value.
         */
        JsonWriter& value(float value) override;

        /**
         * @brief Writes a 64-bit floating point value.
         */
        JsonWriter& value(double value) override;

        /**
         * @brief Writes a long double-precision floating point value.
         */
        JsonWriter& value(long double value);

        /**
         * @brief Writes a wide string value encoded as UTF-8.
         *
         * Characters are automatically escaped if necessary.
         */
        JsonWriter& value(std::wstring_view value) override;

        /**
         * @brief Writes a UTF-8 value.
        *
         * Characters are automatically escaped if necessary.
         */
        JsonWriter& value(std::string_view value) override;

        /**
         * @brief Writes @a data encoded as BASE64.
         *
         * This function is equivalent to calling base64(data, size). The
         * reason it exists is to support the same interface as writers for
         * binary JSON.
         */
        JsonWriter& binary(const void* data, size_t size) override;

        /**
         * @brief Writes @a data encoded as BASE64.
         */
        JsonWriter& base64(const void* data, size_t size) override;

        /**
         * @brief Writes a raw string value.
         *
         * The string is written as-is, without quotation marks or escaped
         * character, but otherwise normal formatting is performed.
         */
        JsonWriter& rawValue(std::string_view value);

        ///@}

        /**
         * @name Functions for retrieving the stream or buffer
         */
        ///@{

        /**
         * @brief Returns the stream the writer writes to.
         *
         * This function returns nullptr if the writer writes to a buffer.
         */
        [[nodiscard]]
        std::ostream* stream() override;

        /**
         * @brief Returns the buffer the writer writes to and the current
         *   size of the buffer.
         *
         * This function returns nullptr if the writer writes to a stream.
         */
        [[nodiscard]]
        std::pair<const void*, size_t> buffer() const override;

        /**
         * @brief Returns the current buffer as a string.
         *
         * This function returns an empty string if the writer writes to a
         * stream.
         */
        [[nodiscard]]
        const std::string& str() const;

        /**
         * @brief Flushes the internal buffer to the stream if there
         *  is a stream.
         *
         * This function is called automatically when the writer is
         * destroyed, but it can be called manually if needed.
         */
        JsonWriter& flush() override;

        ///@}

        /**
         * @name Functions for controlling formatting
         */
        ///@{

        /**
         * @brief Returns the character used for indentation.
         *
         * The second value is the number of times the character is
         * repeated for each level of indentation.
         */
        [[nodiscard]]
        std::pair<char, unsigned> indentation() const;

        /**
         * @brief Sets the character used for indentation.
         *
         * @a width is the number of times the character is
         * repeated for each level of indentation.
         */
        JsonWriter& setIndentation(char character, unsigned width);

        /**
         * @brief Returns the floating point precision.
         */
        [[nodiscard]]
        int floatingPointPrecision() const;

        /**
         * @brief Sets the floating point precision.
         *
         * The precision is the total number of digits before the exponent.
         * The default value is 9.
         */
        JsonWriter& setFloatingPointPrecision(int value);

        ///@}

        /**
         * @name Language extensions
         */
        ///@{

        /**
         * @brief Returns true if special floating point values will be
         *      written as values.
         *
         * When this flag is enabled, the special values not-a-number and
         * positive and negative infinity are written as "NaN", "Infinity"
         * and "-Infinity" respectively. If it is disabled, the write
         * functions will throw an exception if it encounters any of these
         * values.
         */
        [[nodiscard]]
        bool isNonFiniteFloatsEnabled() const;

        /**
         * @brief Sets whether non-finite floating point values (NaN,
         *      Infinity) will be written as values.
         *
         * Unquoted (JSON5-compatible) values are the default.
         */
        JsonWriter& setNonFiniteFloatsEnabled(bool value);

        /**
         * @brief Returns true if special floating point values will be
         *      written as quoted strings.
         *
         * When this flag is enabled, the special values not-a-number and
         * positive and negative infinity are written as "NaN", "Infinity"
         * and "-Infinity" respectively. If it is disabled, the write
         * functions will throw an exception if it encounters any of these
         * values.
         */
        [[nodiscard]]
        bool isQuotedNonFiniteFloatsEnabled() const;

        /**
         * @brief Sets whether non-finite floating point values (NaN,
         *      Infinity) will be surrounded by double-quotes.
         *
         * Unquoted (JSON5-compatible) values are the default.
         */
        JsonWriter& setQuotedNonFiniteFloatsEnabled(bool value);

        /**
         * @brief Returns true if unquoted value names (keys) are enabled.
         *
         * When this flag is enabled, key that are valid JavaScript
         * identifiers are written as unquoted strings. If it is disabled, the value names are
         * written as quoted strings.
         */
        [[nodiscard]]
        bool isUnquotedValueNamesEnabled() const;

        /**
         * @brief Sets whether unquoted value names (keys) are enabled.
         */
        JsonWriter& setUnquotedValueNamesEnabled(bool value);

        /**
         * @brief Returns true if non-ASCII characters are escaped.
         *
         * When this flag is enabled, non-ASCII characters are escaped
         * using the \uXXXX syntax. If it is disabled, the characters are
         * written as-is.
         */
        [[nodiscard]]
        bool isEscapeNonAsciiCharactersEnabled() const;

        /**
         * @brief Sets whether non-ASCII characters are escaped.
         *
         * When this flag is enabled, non-ASCII characters are escaped
         * using the \uXXXX syntax. If it is disabled, the characters are
         * written as-is.
         */
        JsonWriter& setEscapeNonAsciiCharactersEnabled(bool value);

        /**
         * @brief Returns true if multi-line strings are enabled.
         *
         * When this flag is enabled, strings that are longer than the
         * maximum line width will be written as multi-line strings where
         * each line ends with a backslash.
         */
        [[nodiscard]]
        bool isMultilineStringsEnabled() const;

        /**
         * @brief Sets whether multi-line strings are enabled.
         *
         * When this flag is enabled, strings that are longer than the
         * maximum line width will be written as multi-line strings where
         * each line ends with a backslash.
         */
        JsonWriter& setMultilineStringsEnabled(bool value);

        /**
         * @brief Returns the maximum line width used for multi-line strings.
         */
        [[nodiscard]]
        int maximumLineWidth() const;

        /**
         * @brief Sets the maximum line width used for multi-line strings.
         *
         * The default value is 120. The minimum value is 8.
         */
        JsonWriter& setMaximumLineWidth(int value);

        /**
         * @brief Returns the language extensions that are enabled.
         *
         * The return value is a bit mask where each bit represents a
         * language extension. See the LanguageExtensions enum for
         * details.
         */
        [[nodiscard]]
        int languageExtensions() const;

        /**
         * @brief Sets the language extensions that are enabled.
         *
         * The value is a bit mask where each bit represents a language
         * extension. See the LanguageExtensions enum for details.
         */
        JsonWriter& setLanguageExtensions(int value);

        ///@}

        /**
         * @name Functions for manual formatting
         * It is recommended to use the automatic formatting, but if
         * manual control is needed, these functions can be used.
         */
        ///@{

        /**
         * @brief Writes a raw string.
         *
         * The string is written as-is, without quotation marks or escaped
         * character, and without any formatting. The key is ignored and
         * will not be written, if a key is necessary it should be part of
         * @a value.
         *
         * This function is useful for writing contents that is already
         * formatted as JSON.
         */
        JsonWriter& rawText(std::string_view value);

        /**
         * @brief Increase indentation level by one.
         *
         * @note The indentation isn't written until the next value is
         *  written or the writeIndentation() function is called.
         */
        JsonWriter& indent();

        /**
         * @brief Decrease indentation level by one.
         */
        JsonWriter& outdent();

        /**
         * @brief Writes a comma.
         */
        JsonWriter& writeComma();

        /**
         * @brief Writes the indentation.
         */
        JsonWriter& writeIndentation();

        /**
         * @brief Writes a newline.
         */
        JsonWriter& writeNewline();

        /**
         * @brief Writes the given number of spaces.
         */
        JsonWriter& writeSeparator(size_t count = 1);

        /**
         * @brief Returns true if formatting is enabled.
         */
        [[nodiscard]]
        bool isFormattingEnabled() const;

        /**
         * @brief Sets whether formatting is enabled.
         *
         * Use this function to temporarily disable formatting. When
         * formatting is re-enabled, it is set back to the previous value
         * unless it has been changed explicitly in the meantime.
         */
        JsonWriter& setFormattingEnabled(bool value);

        ///@}
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
        JsonWriter& writeIntValueImpl(T number);

        template <typename T>
        JsonWriter& writeFloatValueImpl(T number, const char* format);

        enum LanguageExtensions
        {
            NON_FINITE_FLOATS = 1,
            QUOTED_NON_FINITE_FLOATS = 2,
            MULTILINE_STRINGS = 4,
            UNQUOTED_VALUE_NAMES = 8,
            ESCAPE_NON_ASCII_CHARACTERS = 16
        };

        [[nodiscard]]
        bool languageExtension(LanguageExtensions ext) const;

        JsonWriter& setLanguageExtension(LanguageExtensions ext, bool value);

        std::unique_ptr<Members> m_Members;
    };
}
