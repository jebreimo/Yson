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
    class YSON_API Writer
    {
    public:
        enum Formatting {DEFAULT, NONE, FLAT, FORMAT};

        enum IntegerMode {DECIMAL, BINARY, OCTAL, HEXADECIMAL};

        Writer();

        Writer(const std::string& fileName);

        Writer(std::ostream& stream);

        Writer(std::unique_ptr<std::ostream>&& stream);

        Writer(Writer&& rhs);

        ~Writer();

        Writer& operator=(Writer&& rhs);

        bool isFormattingEnabled() const;

        Writer& setFormattingEnabled(bool value);

        std::pair<char, unsigned> indentation() const;

        Writer& setIndentation(char character, unsigned width);

        std::ostream* stream();

        const std::string& valueName() const;

        Writer& setValueName(const std::string& name);

        Writer& writeBeginArray(Formatting formatting = DEFAULT);

        Writer& writeBeginArray(const std::string& name,
                                Formatting formatting = DEFAULT);

        Writer& writeEndArray();

        Writer& writeBeginObject(Formatting formatting = DEFAULT);

        Writer& writeBeginObject(const std::string& name,
                                 Formatting formatting = DEFAULT);

        Writer& writeEndObject();

        Writer& writeNull();

        Writer& writeNull(const std::string& name);

        Writer& writeBool(bool value);

        Writer& writeBool(const std::string& name, bool value);

        Writer& writeValue(int8_t value);

        Writer& writeValue(int16_t value);

        Writer& writeValue(int32_t value);

        Writer& writeValue(int64_t value);

        Writer& writeValue(uint8_t value);

        Writer& writeValue(uint16_t value);

        Writer& writeValue(uint32_t value);

        Writer& writeValue(uint64_t value);

        Writer& writeValue(float value);

        Writer& writeValue(double value);

        Writer& writeValue(long double value);

        Writer& writeValue(const std::string& value);

        Writer& writeValue(const std::wstring& value);

        template <typename T>
        Writer& writeValue(const std::string& name, const T& value)
        {
            setValueName(name);
            return this->writeValue(value);
        }

        Writer& writeRawValue(const std::string& value);

        Writer& writeRawValue(const std::string& name,
                              const std::string& value);

        Writer& writeRawText(const std::string& text);

        Writer& writeBase64(const void* data, size_t size);

        Writer& writeBase64(const std::string& name,
                            const void* data, size_t size);

        Writer& indent();

        Writer& outdent();

        Writer& writeComma();

        Writer& writeIndentation();

        Writer& writeNewline();

        Writer& writeSeparator(size_t count = 1);

        int languageExtensions() const;

        Writer& setLanguageExtensions(int value);

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

        Writer& setNonFiniteFloatsAsStringsEnabled(bool value);

        bool isUnquotedValueNamesEnabled() const;

        Writer& setUnquotedValueNamesEnabled(bool value);

        IntegerMode integerMode() const;

        Writer& setIntegerMode(IntegerMode mode);
    private:
        void beginValue();

        Formatting formatting() const;

        bool isInsideObject() const;

        Writer& writeBeginStructure(char startChar, char endChar,
                                    Formatting formatting);

        void writeEndStructure(char endChar);

        void writeIndentationImpl();

        template <typename T>
        Writer& writeIntValueImpl(T value);

        template <typename T>
        Writer& writeFloatValueImpl(T value);

        enum LanguageExtensions
        {
            NON_FINITE_FLOATS_AS_STRINGS = 1,
            INTEGERS_AS_BIN = 2,
            INTEGERS_AS_OCT = 4,
            INTEGERS_AS_HEX = 6,
            UNQUOTED_VALUE_NAMES = 8
        };

        bool languageExtension(LanguageExtensions ext) const;

        Writer& setLanguageExtension(LanguageExtensions ext, bool value);

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
