//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonWriter.hpp"

#include <iostream>
#include <stdexcept>
#include <Ystring/Utf8.hpp>

#define JSONWRITER_THROW(msg) \
    throw std::logic_error(msg)

namespace Yson
{
    JsonWriter::JsonWriter()
        : m_Stream(&std::cout),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {}

    JsonWriter::JsonWriter(std::ostream& stream)
        : m_Stream(&stream),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {}

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream>&& stream)
        : m_StreamPtr(std::move(stream)),
          m_Stream(stream.get()),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {}

    JsonWriter::JsonWriter(JsonWriter&& rhs)
        : m_StreamPtr(std::move(rhs.m_StreamPtr)),
          m_Stream(rhs.m_Stream),
          m_Context(std::move(rhs.m_Context)),
          m_ValueName(std::move(rhs.m_ValueName)),
          m_Indentation(rhs.m_Indentation),
          m_IndentationWidth(rhs.m_IndentationWidth),
          m_FormattingEnabled(rhs.m_FormattingEnabled),
          m_IndentationCharacter(rhs.m_IndentationCharacter)
    {
        rhs.m_Stream = nullptr;
    }

    JsonWriter::~JsonWriter()
    {}

    JsonWriter& JsonWriter::operator=(JsonWriter&& rhs)
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

    bool JsonWriter::isFormattingEnabled() const
    {
        return m_FormattingEnabled;
    }

    JsonWriter& JsonWriter::setFormattingEnabled(bool value)
    {
        m_FormattingEnabled = value;
        return *this;
    }

    std::pair<char, unsigned> JsonWriter::indentation() const
    {
        return std::make_pair(m_IndentationCharacter, m_IndentationWidth);
    }

    JsonWriter& JsonWriter::setIndentation(char character, unsigned count)
    {
        m_IndentationCharacter = character;
        m_IndentationWidth = count;
        return *this;
    }

    std::ostream* JsonWriter::stream()
    {
        return m_Stream;
    }

    const std::string& JsonWriter::valueName() const
    {
        return m_ValueName;
    }

    JsonWriter& JsonWriter::setValueName(const std::string& name)
    {
        m_ValueName = name;
        return *this;
    }

    JsonWriter& JsonWriter::writeBeginArray(Formatting formatting)
    {
        return writeBeginStructure('[', ']', formatting);
    }

    JsonWriter& JsonWriter::writeBeginArray(const std::string& name,
                                            Formatting formatting)
    {
        setValueName(name);
        writeBeginArray(formatting);
        return *this;
    }

    JsonWriter& JsonWriter::writeEndArray()
    {
        writeEndStructure(']');
        return *this;
    }

    JsonWriter& JsonWriter::writeBeginObject(Formatting formatting)
    {
      return writeBeginStructure('{', '}', formatting);
    }

    JsonWriter& JsonWriter::writeBeginObject(const std::string& name,
                                             Formatting formatting)
    {
        setValueName(name);
        writeBeginObject(formatting);
        return *this;
    }

    JsonWriter& JsonWriter::writeEndObject()
    {
        writeEndStructure('}');
        return *this;
    }

    JsonWriter& JsonWriter::writeNull()
    {
        writeValue("null");
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(bool value)
    {
        writeValue(value ? "true" : "false");
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(int8_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(int16_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(int32_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(int64_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(uint8_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(uint16_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(uint32_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(uint64_t value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(float value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(double value)
    {
        return writeValueImpl(value);
    }

    JsonWriter& JsonWriter::writeValue(const char* value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(const std::string& value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(const std::wstring& value)
    {
        beginValue();
        *m_Stream << "\""
                  << Ystring::Utf8::toUtf8(value, Ystring::Encoding::UTF_16)
                  << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::writeRawValue(const std::string& value)
    {
        return writeValueImpl(value);
    }


    JsonWriter& JsonWriter::writeRawValue(const std::string& name,
                                          const std::string& value)
    {
        setValueName(name);
        return writeRawValue(value);
    }

    JsonWriter& JsonWriter::writeRawText(const std::string& text)
    {
        *m_Stream << text;
        return *this;
    }

    JsonWriter& JsonWriter::indent()
    {
        ++m_Indentation;
        return *this;
    }

    JsonWriter& JsonWriter::outdent()
    {
        if (m_Indentation == 0)
            JSONWRITER_THROW("Can't outdent, indentation level is 0.");
        --m_Indentation;
        return *this;
    }

    JsonWriter& JsonWriter::writeComma()
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
            JSONWRITER_THROW("A comma has already been added.");
        }
        *m_Stream << ',';
        m_State = AFTER_COMMA;
        return *this;
    }

    JsonWriter& JsonWriter::writeIndentation()
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

    JsonWriter& JsonWriter::writeNewline()
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
        }
        return *this;
    }

    JsonWriter& JsonWriter::writeSeparator(size_t count)
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

    void JsonWriter::beginValue()
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
            *m_Stream << "\"" << m_ValueName << "\": ";
    }

    JsonWriter::Formatting JsonWriter::formatting() const
    {
        if (!m_FormattingEnabled)
            return NONE;
        if (m_Context.empty())
            return FORMAT;
        return m_Context.top().formatting;
    }

    bool JsonWriter::isInsideObject() const
    {
        return !m_Context.empty() && m_Context.top().endChar == '}';
    }

    JsonWriter& JsonWriter::writeBeginStructure(char startChar, char endChar,
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

    void JsonWriter::writeEndStructure(char endChar)
    {
        if (m_Context.empty() || m_Context.top().endChar != endChar)
            JSONWRITER_THROW(std::string("Incorrect position for '")
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
            JSONWRITER_THROW(std::string("Incorrect position for '")
                             + endChar + "'");
        }

        m_Context.pop();
        *m_Stream << endChar;
        m_State = AT_END_OF_VALUE;
    }

    void JsonWriter::writeIndentationImpl()
    {
        auto n = m_Indentation * m_IndentationWidth;
        for (size_t i = 0; i < n; ++i)
            *m_Stream << m_IndentationCharacter;
    }

    template <typename T>
    JsonWriter& JsonWriter::writeValueImpl(T value)
    {
        beginValue();
        *m_Stream << value;
        m_State = AT_END_OF_VALUE;
        return *this;
    }
}
