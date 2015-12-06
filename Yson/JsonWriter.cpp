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
#include "JsonValue.hpp"

namespace Yson
{
    namespace
    {
        void write(JsonWriter& writer,
                   const JsonObject& object,
                   JsonWriter::Formatting formatting);
        void write(JsonWriter& writer,
                   const std::vector<JsonValue>& array,
                   JsonWriter::Formatting formatting);
    }

    JsonWriter::JsonWriter()
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(&std::cout)
    {
    }

    JsonWriter::JsonWriter(std::ostream& stream)
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(&stream)
    {
    }

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream> stream)
        : m_IsFirst(true),
          m_Indentation("  "),
          m_Stream(stream.get()),
          m_StreamPtr(std::move(stream))
    {
    }

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

    void JsonWriter::setIndentation(const std::string& indentation)
    {
        m_Indentation = indentation;
    }

    const std::string& JsonWriter::valueName() const
    {
        return m_ValueName;
    }

    void JsonWriter::setValueName(const std::string& name)
    {
        m_ValueName = name;
    }

    void JsonWriter::beginArray(Formatting formatting)
    {
        beginValue();
        *m_Stream << "[";

        m_IsFirst = true;
        m_Context.push(Context(']', shouldFormat(formatting)));
    }

    void JsonWriter::beginArray(const std::string& name, Formatting formatting)
    {
        setValueName(name);
        beginArray(formatting);
    }

    void JsonWriter::endArray()
    {
        endStructure(']');
    }

    void JsonWriter::beginObject(Formatting formatting)
    {
        beginValue();
        *m_Stream << "{";

        m_IsFirst = true;
        m_Context.push(Context('}', shouldFormat(formatting)));
    }

    void JsonWriter::beginObject(const std::string& name, Formatting formatting)
    {
        setValueName(name);
        beginObject(formatting);
    }

    void JsonWriter::endObject()
    {
        endStructure('}');
    }

    void JsonWriter::newline(bool comma)
    {
        if (comma && !m_IsFirst)
            *m_Stream << ",\n";
        else
            *m_Stream << "\n";

        indent();
        m_IsFirst = true;
    }

    void JsonWriter::value()
    {
        writeValue("null");
    }

    void JsonWriter::value(int32_t value)
    {
        writeValue(value);
    }

    void JsonWriter::value(int64_t value)
    {
        writeValue(value);
    }

    void JsonWriter::value(uint32_t value)
    {
        writeValue(value);
    }

    void JsonWriter::value(uint64_t value)
    {
        writeValue(value);
    }

    void JsonWriter::value(double value)
    {
        writeValue(value);
    }

    void JsonWriter::value(const char* value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_IsFirst = false;
    }

    void JsonWriter::value(const std::string& value)
    {
        beginValue();
        *m_Stream << "\"" << value << "\"";
        m_IsFirst = false;
    }

    void JsonWriter::value(const wchar_t* _value)
    {
        value(std::wstring(_value));
    }

    void JsonWriter::value(const std::wstring& value)
    {
        beginValue();
        *m_Stream << "\"" << Ystring::Utf8::toUtf8(value, Ystring::Encoding::UTF_16) << "\"";
        m_IsFirst = false;
    }

    void JsonWriter::value(bool value)
    {
        writeValue(value ? "true" : "false");
    }

    void JsonWriter::value(const JsonValue& jsonValue, Formatting formatting)
    {
        switch (jsonValue.type())
        {
        case JsonValue::OBJECT:
            write(*this, jsonValue.object(), formatting);
            break;
        case JsonValue::ARRAY:
            write(*this, jsonValue.array(), formatting);
            break;
        case JsonValue::STRING:
            value(jsonValue.string());
            break;
        case JsonValue::REAL:
            value(jsonValue.real());
            break;
        case JsonValue::INTEGER:
            value(jsonValue.integer());
            break;
        case JsonValue::BOOLEAN:
            value(jsonValue.boolean());
            break;
        case JsonValue::NULL_VALUE:
        default:
            value();
            break;
        }
    }

    void JsonWriter::value(const std::string& name, const JsonValue& val, Formatting formatting)
    {
        setValueName(name);
        value(val, formatting);
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

    namespace
    {
        void write(JsonWriter& writer,
                   const JsonObject& object,
                   JsonWriter::Formatting formatting)
        {
            writer.beginObject(formatting);
            for (auto it = object.begin(); it != object.end(); it++)
                writer.value(it->first, it->second, formatting);
            writer.endObject();
        }

        void write(JsonWriter& writer,
                   const std::vector<JsonValue>& array,
                   JsonWriter::Formatting formatting)
        {
            writer.beginArray(formatting);
            for (auto it = array.begin(); it != array.end(); it++)
                writer.value(*it, formatting);
            writer.endArray();
        }
    }
}
