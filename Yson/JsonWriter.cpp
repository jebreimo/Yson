//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonWriter.hpp"

#include <cstdint>
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

    JsonWriter& JsonWriter::beginArray(Formatting formatting)
    {
        beginValue();
        *m_Stream << "[";

        m_IsFirst = true;
        m_Context.push(Context(']', shouldFormat(formatting)));
        return *this;
    }

    JsonWriter& JsonWriter::beginArray(const std::string& name, Formatting formatting)
    {
        setValueName(name);
        beginArray(formatting);
        return *this;
    }

    JsonWriter& JsonWriter::endArray()
    {
        endStructure(']');
        return *this;
    }

    JsonWriter& JsonWriter::beginObject(Formatting formatting)
    {
        beginValue();
        *m_Stream << "{";

        m_IsFirst = true;
        m_Context.push(Context('}', shouldFormat(formatting)));
        return *this;
    }

    JsonWriter& JsonWriter::beginObject(const std::string& name, Formatting formatting)
    {
        setValueName(name);
        beginObject(formatting);
        return *this;
    }

    JsonWriter& JsonWriter::endObject()
    {
        endStructure('}');
        return *this;
    }

    JsonWriter& JsonWriter::newline(bool comma)
    {
        if (comma && !m_IsFirst)
            *m_Stream << ",\n";
        else
            *m_Stream << "\n";

        indent();
        m_IsFirst = true;
        return *this;
    }

    JsonWriter& JsonWriter::value()
    {
        writeValue("null");
        return *this;
    }

    JsonWriter& JsonWriter::value(int32_t value)
    {
        writeValue(value);
        return *this;
    }

    JsonWriter& JsonWriter::value(int64_t value)
    {
        writeValue(value);
        return *this;
    }

    JsonWriter& JsonWriter::value(uint32_t value)
    {
        writeValue(value);
        return *this;
    }

    JsonWriter& JsonWriter::value(uint64_t value)
    {
        writeValue(value);
        return *this;
    }

    JsonWriter& JsonWriter::value(double value)
    {
        writeValue(value);
        return *this;
    }

    JsonWriter& JsonWriter::value(const char* value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_IsFirst = false;
        return *this;
    }

    JsonWriter& JsonWriter::value(const std::string& value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_IsFirst = false;
        return *this;
    }

    JsonWriter& JsonWriter::value(const wchar_t* _value)
    {
        value(std::wstring(_value));
        return *this;
    }

    JsonWriter& JsonWriter::value(const std::wstring& value)
    {
        beginValue();
        *m_Stream << "\""
                  << Ystring::Utf8::toUtf8(value, Ystring::Encoding::UTF_16)
                  << "\"";
        m_IsFirst = false;
        return *this;
    }

    JsonWriter& JsonWriter::value(bool value)
    {
        writeValue(value ? "true" : "false");
        return *this;
    }

    void JsonWriter::beginValue()
    {
        if (!m_Context.empty() && m_Context.top().format)
        {
            if (!m_IsFirst)
                *m_Stream << ',';
            *m_Stream << '\n';
            indent();
        }
        else if (!m_IsFirst)
            *m_Stream << ", ";

        if (!m_Context.empty() && m_Context.top().endChar == '}')
            *m_Stream << "\"" << m_ValueName << "\": ";
    }

    void JsonWriter::endStructure(char endChar)
    {
        if (m_Context.empty() || m_Context.top().endChar != endChar)
            throw std::logic_error(std::string("Incorrect position for '") + endChar + "'");

        bool format = m_Context.top().format;
        m_Context.pop();
        if (!m_IsFirst && format)
            newline(false);

        *m_Stream << endChar;
        m_IsFirst = false;
    }

    void JsonWriter::indent()
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
    void JsonWriter::writeValue(T value)
    {
        beginValue();
        *m_Stream << value;
        m_IsFirst = false;
    }
}
