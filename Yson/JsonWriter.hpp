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
#include <stack>
#include "Writer.hpp"
#include "YsonDefinitions.hpp"

namespace Yson
{
    class YSON_API JsonWriter : public Writer
    {
    public:
        JsonWriter(JsonFormatting formatting = JsonFormatting::NONE);

        JsonWriter(const std::string& fileName,
                   JsonFormatting formatting = JsonFormatting::NONE);

        JsonWriter(std::ostream& stream,
                   JsonFormatting formatting = JsonFormatting::NONE);

        JsonWriter(const JsonWriter&) = delete;

        JsonWriter(JsonWriter&& rhs);

        ~JsonWriter();

        JsonWriter& operator=(const JsonWriter&) = delete;

        JsonWriter& operator=(JsonWriter&& rhs);

        std::ostream* stream();

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
          *     written as strings.
          *
          * When this flag is enabled, the special values not-a-number and
          * positive and negative infinity are written as "NaN", "infinity"
          * and "-infinity" respectively. If it is disabled, the write
          * functions will throw an exception if it encounters any of these
          * values.
          */
        bool isNonFiniteFloatsAsStringsEnabled() const;

        JsonWriter& setNonFiniteFloatsAsStringsEnabled(bool value);

        bool isUnquotedValueNamesEnabled() const;

        JsonWriter& setUnquotedValueNamesEnabled(bool value);

    private:
        JsonWriter(std::unique_ptr<std::ostream>&& streamPtr,
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
        JsonWriter& writeIntValueImpl(T number);

        template <typename T>
        JsonWriter& writeFloatValueImpl(T number);

        enum LanguageExtensions
        {
            NON_FINITE_FLOATS_AS_STRINGS = 1,
            UNQUOTED_VALUE_NAMES = 8
        };

        bool languageExtension(LanguageExtensions ext) const;

        JsonWriter& setLanguageExtension(LanguageExtensions ext, bool value);

        struct Context
        {
            Context()
            {}

            Context(char endChar,
                    JsonParameters parameters,
                    size_t valueIndex = 0)
                : parameters(parameters),
                  valueIndex(valueIndex),
                  endChar(endChar)
            {}

            JsonParameters parameters;
            size_t valueIndex = 0;
            char endChar = 0;
        };

        enum State
        {
            AT_START_OF_VALUE_NO_COMMA,
            AT_START_OF_VALUE_AFTER_COMMA,
            AT_START_OF_STRUCTURE,
            AT_END_OF_VALUE,
            AT_START_OF_LINE_NO_COMMA,
            AT_START_OF_LINE_AFTER_COMMA,
            AT_START_OF_LINE_BEFORE_COMMA,
            AFTER_COMMA
        };

        std::unique_ptr<std::ostream> m_StreamPtr;
        std::ostream* m_Stream;
        std::stack<Context> m_Contexts;
        std::string m_Key;
        State m_State;
        unsigned m_Indentation;
        unsigned m_IndentationWidth;
        int m_LanguagExtensions;
        bool m_FormattingEnabled;
        char m_IndentationCharacter;
    };
}
