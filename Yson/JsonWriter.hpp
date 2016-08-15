//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <stack>
#include "YsonDefinitions.hpp"

namespace Yson
{

    class YSON_API JsonWriter
    {
    public:
        enum Formatting {DEFAULT, NONE, FLAT, FORMAT};
        enum IntegerMode {DECIMAL, BINARY, OCTAL, HEXADECIMAL};

        JsonWriter();

        JsonWriter(const std::string& fileName);

        JsonWriter(std::ostream& stream);

        JsonWriter(std::unique_ptr<std::ostream>&& stream);

        JsonWriter(JsonWriter&& rhs);

        ~JsonWriter();

        JsonWriter& operator=(JsonWriter&& rhs);

        bool isFormattingEnabled() const;

        JsonWriter& setFormattingEnabled(bool value);

        std::pair<char, unsigned> indentation() const;

        JsonWriter& setIndentation(char character, unsigned width);

        std::ostream* stream();

        const std::string& valueName() const;

        JsonWriter& setValueName(const std::string& name);

        JsonWriter& writeBeginArray(Formatting formatting = DEFAULT);

        JsonWriter& writeBeginArray(const std::string& name,
                                    Formatting formatting = DEFAULT);

        JsonWriter& writeEndArray();

        JsonWriter& writeBeginObject(Formatting formatting = DEFAULT);

        JsonWriter& writeBeginObject(const std::string& name,
                                     Formatting formatting = DEFAULT);

        JsonWriter& writeEndObject();

        JsonWriter& writeNull();

        JsonWriter& writeNull(const std::string& name);

        JsonWriter& writeBool(bool value);

        JsonWriter& writeBool(const std::string& name, bool value);

        JsonWriter& writeValue(int8_t value);

        JsonWriter& writeValue(int16_t value);

        JsonWriter& writeValue(int32_t value);

        JsonWriter& writeValue(int64_t value);

        JsonWriter& writeValue(uint8_t value);

        JsonWriter& writeValue(uint16_t value);

        JsonWriter& writeValue(uint32_t value);

        JsonWriter& writeValue(uint64_t value);

        JsonWriter& writeValue(float value);

        JsonWriter& writeValue(double value);

        JsonWriter& writeValue(long double value);

        JsonWriter& writeValue(const std::string& value);

        JsonWriter& writeValue(const std::wstring& value);

        template <typename T>
        JsonWriter& writeValue(const std::string& name, const T& value)
        {
            setValueName(name);
            return this->writeValue(value);
        }

        JsonWriter& writeRawValue(const std::string& value);

        JsonWriter& writeRawValue(const std::string& name,
                                  const std::string& value);

        JsonWriter& writeRawText(const std::string& text);

        JsonWriter& writeBase64(const void* data, size_t size);

        JsonWriter& writeBase64(const std::string& name,
                                const void* data, size_t size);

        JsonWriter& indent();

        JsonWriter& outdent();

        JsonWriter& writeComma();

        JsonWriter& writeIndentation();

        JsonWriter& writeNewline();

        JsonWriter& writeSeparator(size_t count = 1);

        int languageExtensions() const;

        JsonWriter& setLanguageExtensions(int value);

        /** @brief Returns true if special floating point values will be
          *     written as strings.
          *
          * When this flag is enabled, the special values not-a-number and
          * positive and negative infinity are written as "NaN", "infinity"
          * and "-infinity" respectively. If it is disabled, the writeValue
          * functions will throw an exception if it encounters any of these
          * values.
          */
        bool isNonFiniteFloatsAsStringsEnabled() const;

        JsonWriter& setNonFiniteFloatsAsStringsEnabled(bool value);

        bool isUnquotedValueNamesEnabled() const;

        JsonWriter& setUnquotedValueNamesEnabled(bool value);

        IntegerMode integerMode() const;

        JsonWriter& setIntegerMode(IntegerMode mode);
    private:
        void beginValue();
        Formatting formatting() const;
        bool isInsideObject() const;
        JsonWriter& writeBeginStructure(char startChar, char endChar,
                                        Formatting formatting);
        void writeEndStructure(char endChar);
        void writeIndentationImpl();

        template <typename T>
        JsonWriter& writeIntValueImpl(T value);

        template <typename T>
        JsonWriter& writeFloatValueImpl(T value);

        enum LanguageExtensions
        {
            NON_FINITE_FLOATS_AS_STRINGS = 1,
            INTEGERS_AS_BIN = 2,
            INTEGERS_AS_OCT = 4,
            INTEGERS_AS_HEX = 6,
            UNQUOTED_VALUE_NAMES = 8
        };

        bool languageExtension(LanguageExtensions ext) const;

        JsonWriter& setLanguageExtension(LanguageExtensions ext, bool value);

        struct Context
        {
            Context()
                : endChar(0),
                  formatting(NONE)
            {}

            Context(char endChar, Formatting formatting)
                : endChar(endChar),
                  formatting(formatting)
            {}

            char endChar;
            Formatting formatting;
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
        std::stack<Context> m_Context;
        std::string m_ValueName;
        State m_State;
        unsigned m_Indentation;
        unsigned m_IndentationWidth;
        int m_LanguagExtensions;
        bool m_FormattingEnabled;
        char m_IndentationCharacter;
    };
}
