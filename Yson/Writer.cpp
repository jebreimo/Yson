//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Writer.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <type_traits>
#include "../Ystring/Conversion.hpp"
#include "../Ystring/Escape/Escape.hpp"
#include "Base64.hpp"
#include "IsJavaScriptIdentifier.hpp"
#include "YsonException.hpp"

namespace Yson
{
    Writer::Writer()
        : m_Stream(&std::cout),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_LanguagExtensions(0),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {}

    Writer::Writer(const std::string& fileName)
        : m_StreamPtr(new std::ofstream(fileName)),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_LanguagExtensions(0),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {
        m_Stream = m_StreamPtr.get();
    }

    Writer::Writer(std::ostream& stream)
        : m_Stream(&stream),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_LanguagExtensions(0),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {}

    Writer::Writer(std::unique_ptr<std::ostream>&& stream)
        : m_StreamPtr(std::move(stream)),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_LanguagExtensions(0),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {
        m_Stream = m_StreamPtr.get();
    }

    Writer::Writer(Writer&& rhs)
        : m_StreamPtr(std::move(rhs.m_StreamPtr)),
          m_Context(std::move(rhs.m_Context)),
          m_ValueName(std::move(rhs.m_ValueName)),
          m_Indentation(rhs.m_Indentation),
          m_IndentationWidth(rhs.m_IndentationWidth),
          m_LanguagExtensions(rhs.m_LanguagExtensions),
          m_FormattingEnabled(rhs.m_FormattingEnabled),
          m_IndentationCharacter(rhs.m_IndentationCharacter)
    {
        m_Stream = m_StreamPtr.get();
        rhs.m_Stream = nullptr;
    }

    Writer::~Writer()
    {}

    Writer& Writer::operator=(Writer&& rhs)
    {
        m_StreamPtr = std::move(rhs.m_StreamPtr);
        m_Stream = rhs.m_Stream;
        rhs.m_Stream = nullptr;
        m_Context = std::move(rhs.m_Context);
        m_ValueName = std::move(rhs.m_ValueName);
        m_Indentation = rhs.m_Indentation;
        m_IndentationCharacter = rhs.m_IndentationCharacter;
        m_FormattingEnabled = rhs.m_FormattingEnabled;
        m_IndentationWidth = rhs.m_IndentationWidth;
        return *this;
    }

    bool Writer::isFormattingEnabled() const
    {
        return m_FormattingEnabled;
    }

    Writer& Writer::setFormattingEnabled(bool value)
    {
        m_FormattingEnabled = value;
        return *this;
    }

    std::pair<char, unsigned> Writer::indentation() const
    {
        return std::make_pair(m_IndentationCharacter, m_IndentationWidth);
    }

    Writer& Writer::setIndentation(char character, unsigned count)
    {
        m_IndentationCharacter = character;
        m_IndentationWidth = count;
        return *this;
    }

    std::ostream* Writer::stream()
    {
        return m_Stream;
    }

    const std::string& Writer::valueName() const
    {
        return m_ValueName;
    }

    Writer& Writer::setValueName(const std::string& name)
    {
        m_ValueName = name;
        return *this;
    }

    Writer& Writer::writeBeginArray(Formatting formatting)
    {
        return writeBeginStructure('[', ']', formatting);
    }

    Writer& Writer::writeBeginArray(const std::string& name,
                                    Formatting formatting)
    {
        setValueName(name);
        writeBeginArray(formatting);
        return *this;
    }

    Writer& Writer::writeEndArray()
    {
        writeEndStructure(']');
        return *this;
    }

    Writer& Writer::writeBeginObject(Formatting formatting)
    {
      return writeBeginStructure('{', '}', formatting);
    }

    Writer& Writer::writeBeginObject(const std::string& name,
                                     Formatting formatting)
    {
        setValueName(name);
        writeBeginObject(formatting);
        return *this;
    }

    Writer& Writer::writeEndObject()
    {
        writeEndStructure('}');
        return *this;
    }

    Writer& Writer::writeNull()
    {
        writeValue("null");
        return *this;
    }

    Writer& Writer::writeNull(const std::string& name)
    {
        setValueName(name);
        return writeNull();
    }

    Writer& Writer::writeBool(bool value)
    {
        writeValue(value ? "true" : "false");
        return *this;
    }

    Writer& Writer::writeBool(const std::string& name, bool value)
    {
        setValueName(name);
        return writeBool(value);
    }

    Writer& Writer::writeValue(int8_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(int16_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(int32_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(int64_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(uint8_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(uint16_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(uint32_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(uint64_t value)
    {
        return writeIntValueImpl(value);
    }

    Writer& Writer::writeValue(float value)
    {
        return writeFloatValueImpl(value);
    }

    Writer& Writer::writeValue(double value)
    {
        return writeFloatValueImpl(value);
    }

    Writer& Writer::writeValue(long double value)
    {
        return writeFloatValueImpl(value);
    }

    Writer& Writer::writeValue(const std::string& value)
    {
        beginValue();
        *m_Stream << "\"";
        if (!Ystring::hasUnescapedCharacters(value))
            *m_Stream << value;
        else
            *m_Stream << Ystring::escape(value);
        *m_Stream << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    Writer& Writer::writeValue(const std::wstring& value)
    {
        beginValue();
        auto valueUtf8 = Ystring::Conversion::convert<std::string>(
                value,
                Ystring::Encoding::UTF_16,
                Ystring::Encoding::UTF_8);
        *m_Stream << "\"" << std::move(valueUtf8) << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    Writer& Writer::writeRawValue(const std::string& value)
    {
        beginValue();
        *m_Stream << value;
        m_State = AT_END_OF_VALUE;
        return *this;
    }


    Writer& Writer::writeRawValue(const std::string& name,
                                  const std::string& value)
    {
        setValueName(name);
        return writeRawValue(value);
    }

    Writer& Writer::writeRawText(const std::string& text)
    {
        *m_Stream << text;
        return *this;
    }

    Writer& Writer::writeBase64(const void* data, size_t size)
    {
        return writeValue(toBase64(data, size));
    }

    Writer& Writer::writeBase64(const std::string& name,
                                const void* data, size_t size)
    {
        setValueName(name);
        return writeValue(toBase64(data, size));
    }

    Writer& Writer::indent()
    {
        ++m_Indentation;
        return *this;
    }

    Writer& Writer::outdent()
    {
        if (m_Indentation == 0)
            YSON_THROW("Can't outdent, indentation level is 0.");
        --m_Indentation;
        return *this;
    }

    Writer& Writer::writeComma()
    {
        switch (m_State)
        {
        case AT_END_OF_VALUE:
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            if (formatting() == FORMAT)
                writeIndentationImpl();
            break;
        default:
            YSON_THROW("A comma has already been added.");
        }
        *m_Stream << ',';
        m_State = AFTER_COMMA;
        return *this;
    }

    Writer& Writer::writeIndentation()
    {
        switch (m_State)
        {
        case AT_START_OF_LINE_NO_COMMA:
            writeIndentationImpl();
            m_State = AT_START_OF_VALUE_NO_COMMA;
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            writeIndentationImpl();
            m_State = AT_END_OF_VALUE;
            break;
        case AT_START_OF_LINE_AFTER_COMMA:
            writeIndentationImpl();
            m_State = AT_START_OF_VALUE_AFTER_COMMA;
            break;
        default:
            break;
        }
        return *this;
    }

    Writer& Writer::writeNewline()
    {
        *m_Stream << "\n";
        switch (m_State)
        {
        case AT_START_OF_VALUE_NO_COMMA:
        case AT_START_OF_STRUCTURE:
            m_State = AT_START_OF_LINE_NO_COMMA;
            break;
        case AT_START_OF_VALUE_AFTER_COMMA:
        case AFTER_COMMA:
            m_State = AT_START_OF_LINE_AFTER_COMMA;
            break;
        case AT_END_OF_VALUE:
            m_State = AT_START_OF_LINE_BEFORE_COMMA;
            break;
        default:
            break;
        }
        return *this;
    }

    Writer& Writer::writeSeparator(size_t count)
    {
        if (count == 0)
            return *this;

        switch (m_State)
        {
        case AT_START_OF_LINE_NO_COMMA:
            m_State = AT_START_OF_VALUE_NO_COMMA;
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            m_State = AT_END_OF_VALUE;
            break;
        case AT_START_OF_LINE_AFTER_COMMA:
        case AFTER_COMMA:
            m_State = AT_START_OF_VALUE_AFTER_COMMA;
            break;
        default:
            break;
        }
        for (size_t i = 0; i < count; ++i)
            *m_Stream << m_IndentationCharacter;
        return *this;
    }

    int Writer::languageExtensions() const
    {
        return m_LanguagExtensions;
    }

    Writer& Writer::setLanguageExtensions(int value)
    {
        m_LanguagExtensions = value;
        return *this;
    }

    bool Writer::isNonFiniteFloatsAsStringsEnabled() const
    {
        return languageExtension(NON_FINITE_FLOATS_AS_STRINGS);
    }

    Writer& Writer::setNonFiniteFloatsAsStringsEnabled(bool value)
    {
        return setLanguageExtension(NON_FINITE_FLOATS_AS_STRINGS, value);
    }

    bool Writer::isUnquotedValueNamesEnabled() const
    {
        return languageExtension(UNQUOTED_VALUE_NAMES);
    }

    Writer& Writer::setUnquotedValueNamesEnabled(bool value)
    {
        return setLanguageExtension(UNQUOTED_VALUE_NAMES, value);
    }

    Writer::IntegerMode Writer::integerMode() const
    {
        switch (m_LanguagExtensions & INTEGERS_AS_HEX)
        {
        case INTEGERS_AS_HEX:
            return HEXADECIMAL;
        case INTEGERS_AS_BIN:
            return BINARY;
        case INTEGERS_AS_OCT:
            return OCTAL;
        default:
            return DECIMAL;
        }
    }

    Writer& Writer::setIntegerMode(Writer::IntegerMode mode)
    {
        auto newFlags = m_LanguagExtensions & ~INTEGERS_AS_HEX;
        switch (mode)
        {
        case DECIMAL:
            break;
        case BINARY:
            newFlags |= INTEGERS_AS_BIN;
            break;
        case OCTAL:
            newFlags |= INTEGERS_AS_OCT;
            break;
        case HEXADECIMAL:
            newFlags |= INTEGERS_AS_HEX;
            break;
        }
        m_LanguagExtensions = newFlags;
        return *this;
    }

    void Writer::beginValue()
    {
        switch (m_State)
        {
        case AT_START_OF_STRUCTURE:
            if (formatting() == FORMAT)
            {
                *m_Stream << '\n';
                writeIndentationImpl();
            }
            break;
        case AT_END_OF_VALUE:
            switch (formatting())
            {
            case NONE:
                *m_Stream << ',';
                break;
            case FLAT:
                *m_Stream << ", ";
                break;
            case FORMAT:
                *m_Stream << ",\n";
                writeIndentationImpl();
                break;
            default:
                break;
            }
            break;
        case AT_START_OF_LINE_NO_COMMA:
        case AT_START_OF_LINE_AFTER_COMMA:
            if (formatting() == FORMAT)
                writeIndentationImpl();
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            switch (formatting())
            {
            case NONE:
                *m_Stream << ',';
                break;
            case FLAT:
                *m_Stream << ", ";
                break;
            case FORMAT:
                writeIndentationImpl();
                *m_Stream << ",\n";
                writeIndentationImpl();
                break;
            default:
                break;
            }
            break;
        case AFTER_COMMA:
            switch (formatting())
            {
            case FLAT:
                *m_Stream << ' ';
                break;
            case FORMAT:
                *m_Stream << '\n';
                writeIndentationImpl();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        if (isInsideObject())
        {
            if (!isUnquotedValueNamesEnabled()
                || !isJavaScriptIdentifier(m_ValueName))
                *m_Stream << "\"" << m_ValueName
                          << (formatting() != NONE ? "\": " : "\":");
            else
                *m_Stream << m_ValueName
                          << (formatting() != NONE ? ": " : ":");
        }
    }

    Writer::Formatting Writer::formatting() const
    {
        if (!m_FormattingEnabled)
            return NONE;
        if (m_Context.empty())
            return FORMAT;
        return m_Context.top().formatting;
    }

    bool Writer::isInsideObject() const
    {
        return !m_Context.empty() && m_Context.top().endChar == '}';
    }

    Writer& Writer::writeBeginStructure(char startChar, char endChar,
                                        Formatting formatting)
    {
        beginValue();
        *m_Stream << startChar;
        if (formatting == DEFAULT)
        {
            formatting = m_Context.empty() ? FORMAT
                                           : m_Context.top().formatting;
        }
        if (formatting == FORMAT)
            indent();
        m_Context.push(Context(endChar, formatting));
        m_State = AT_START_OF_STRUCTURE;
        return *this;
    }

    void Writer::writeEndStructure(char endChar)
    {
        if (m_Context.empty() || m_Context.top().endChar != endChar)
            YSON_THROW(std::string("Incorrect position for '")
                       + endChar + "'");

        switch (m_State)
        {
        case AT_START_OF_VALUE_NO_COMMA:
        case AT_START_OF_STRUCTURE:
            if (formatting() == FORMAT)
                outdent();
            break;
        case AT_END_OF_VALUE:
            if (formatting() == FORMAT)
            {
                outdent();
                *m_Stream << '\n';
                writeIndentationImpl();
            }
            break;
        case AT_START_OF_LINE_NO_COMMA:
        case AT_START_OF_LINE_BEFORE_COMMA:
            if (formatting() == FORMAT)
            {
                outdent();
                writeIndentationImpl();
            }
            break;
        default:
            YSON_THROW(std::string("Incorrect position for '")
                       + endChar + "'");
        }

        m_Context.pop();
        *m_Stream << endChar;
        m_State = AT_END_OF_VALUE;
    }

    void Writer::writeIndentationImpl()
    {
        auto n = m_Indentation * m_IndentationWidth;
        for (size_t i = 0; i < n; ++i)
            *m_Stream << m_IndentationCharacter;
    }

    template <typename T>
    void writeBinary(std::ostream& stream, T value)
    {
        stream << "\"0b";
        typedef typename std::make_unsigned<T>::type UnsignedT;
        UnsignedT bit = UnsignedT(1) << (sizeof(UnsignedT) * CHAR_BIT - 1);
        while (bit)
        {
            if (bit & value)
                break;
            bit >>= 1;
        }
        if (!bit)
        {
            stream << '0';
        }
        else
        {
            while (bit)
            {
                stream << ((bit & value) ? '1' : '0');
                bit >>= 1;
            }
        }
        stream << '"';
    }

    template <typename T>
    Writer& Writer::writeIntValueImpl(T value)
    {
        beginValue();
        if (!(m_LanguagExtensions & INTEGERS_AS_HEX))
            *m_Stream << value;
        else if ((m_LanguagExtensions & INTEGERS_AS_HEX) == INTEGERS_AS_HEX)
            *m_Stream << "\"0x" << std::hex << std::uppercase << value << '"' << std::dec;
        else if (m_LanguagExtensions & INTEGERS_AS_OCT)
            *m_Stream << "\"0o" << std::oct << value << '"' << std::dec;
        else if (m_LanguagExtensions & INTEGERS_AS_BIN)
            writeBinary(*m_Stream, value);
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    template <typename T>
    Writer& Writer::writeFloatValueImpl(T value)
    {
        if (std::isfinite(value))
        {
            beginValue();
            *m_Stream << value;
            m_State = AT_END_OF_VALUE;
        }
        else if (!languageExtension(NON_FINITE_FLOATS_AS_STRINGS))
        {
            YSON_THROW(std::string("Illegal floating point value '")
                       + std::to_string(value) + "'");
        }
        else
        {
            if (std::isnan(value))
                writeValue("NaN");
            else if (value < 0)
                writeValue("-infinity");
            else
                writeValue("infinity");
        }
        return *this;
    }

    bool Writer::languageExtension(Writer::LanguageExtensions ext) const
    {
        return (m_LanguagExtensions & ext) != 0;
    }

    Writer& Writer::setLanguageExtension(Writer::LanguageExtensions ext,
                                         bool value)
    {
        if (value)
            m_LanguagExtensions |= ext;
        else
            m_LanguagExtensions &= ~ext;
        return *this;
    }

}
