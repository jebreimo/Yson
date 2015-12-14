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

namespace Yson
{
    JsonWriter::JsonWriter()
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(&std::cout)
    {}

    JsonWriter::JsonWriter(std::ostream& stream)
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(&stream)
    {}

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream> stream)
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(stream.get()),
          m_StreamPtr(std::move(stream))
    {}

    JsonWriter::JsonWriter(JsonWriter&& rhs)
        : m_Context(std::move(rhs.m_Context)),
          m_ValueName(std::move(rhs.m_ValueName)),
          m_IsFirst(rhs.m_IsFirst),
          m_Indentation(std::move(rhs.m_Indentation)),
          m_Stream(rhs.m_Stream),
          m_StreamPtr(std::move(rhs.m_StreamPtr))
    {
        rhs.m_Stream = nullptr;
    }

    JsonWriter::~JsonWriter()
    {}

    JsonWriter& JsonWriter::operator=(JsonWriter&& rhs)
    {
        m_Context = std::move(rhs.m_Context);
        m_ValueName = std::move(rhs.m_ValueName);
        m_IsFirst = rhs.m_IsFirst;
        m_Indentation = std::move(rhs.m_Indentation);
        m_Stream = rhs.m_Stream;
        rhs.m_Stream = nullptr;
        m_StreamPtr = std::move(rhs.m_StreamPtr);
        return *this;
    }

    std::ostream& JsonWriter::stream()
    {
        return &m_Stream;
    }

    const std::string& JsonWriter::indentation() const
    {
        return m_Indentation;
    }

    JsonWriter& JsonWriter::setIndentation(const std::string& indentation)
    {
        m_Indentation = indentation;
        return *this;
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
        beginValue();
        *m_Stream << "[";

        m_IsFirst = true;
        m_Context.push(Context(']', shouldFormat(formatting)));
        return *this;
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
        beginValue();
        *m_Stream << "{";

        m_IsFirst = true;
        m_Context.push(Context('}', shouldFormat(formatting)));
        return *this;
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

    JsonWriter& JsonWriter::writeNewline(bool comma)
    {
        if (comma && !m_IsFirst)
            *m_Stream << ",\n";
        else
            *m_Stream << "\n";

        writeIndent();
        m_IsFirst = true;
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
        m_IsFirst = false;
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(const std::string& value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_IsFirst = false;
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(const wchar_t* _value)
    {
        writeValue(std::wstring(_value));
        return *this;
    }

    JsonWriter& JsonWriter::writeValue(const std::wstring& value)
    {
        beginValue();
        *m_Stream << "\""
                  << Ystring::Utf8::toUtf8(value, Ystring::Encoding::UTF_16)
                  << "\"";
        m_IsFirst = false;
        return *this;
    }

    void JsonWriter::beginValue()
    {
        if (!m_Context.empty() && m_Context.top().format)
        {
            if (!m_IsFirst)
                *m_Stream << ',';
            *m_Stream << '\n';
            writeIndent();
        }
        else if (!m_IsFirst)
            *m_Stream << ", ";

        if (!m_Context.empty() && m_Context.top().endChar == '}')
            *m_Stream << "\"" << m_ValueName << "\": ";
    }

    void JsonWriter::writeEndStructure(char endChar)
    {
        if (m_Context.empty() || m_Context.top().endChar != endChar)
            throw std::logic_error(std::string("Incorrect position for '") + endChar + "'");

        bool format = m_Context.top().format;
        m_Context.pop();
        if (!m_IsFirst && format)
            writeNewline(false);

        *m_Stream << endChar;
        m_IsFirst = false;
    }

    void JsonWriter::writeIndent()
    {
        for (size_t i = 0; i != m_Context.size(); i++)
            *m_Stream << m_Indentation;
    }

    bool JsonWriter::shouldFormat(Formatting formatting)
    {
        switch (formatting)
        {
            case DEFAULT:
                return !m_Context.empty() ? m_Context.top().format : true;
            case FORMATTED:
                return true;
            default:
                return false;
        }
    }

    template <typename T>
    JsonWriter& JsonWriter::writeValueImpl(T value)
    {
        beginValue();
        *m_Stream << value;
        m_IsFirst = false;
        return *this;
    }
}
