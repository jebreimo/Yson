//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../JsonWriter.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include "../../Ystring/Conversion.hpp"
#include "../../Ystring/Escape/Escape.hpp"
#include "../Common/Base64.hpp"
#include "../YsonException.hpp"
#include "IsJavaScriptIdentifier.hpp"

#if defined(WIN32) || defined(WIN64)
    std::unique_ptr<std::ostream> openFile(const std::string& fileName)
    {
        try
        {
            auto fileName16 = Ystring::Conversion::convert<std::wstring>(
                    fileName,
                    Ystring::Encoding::UTF_8,
                    Ystring::Encoding::UTF_16,
                    Ystring::Conversion::ErrorHandlingPolicy::THROW);
            return std::make_unique<std::ofstream>(fileName16);
        }
        catch (Ystring::YstringException&)
        {
            return std::make_unique<std::ofstream>(fileName);
        }
    }
#else
    std::unique_ptr<std::ostream> openFile(const std::string& fileName)
    {
        return std::make_unique<std::ofstream>(fileName);
    }
#endif

namespace Yson
{
    JsonWriter::JsonWriter(JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), &std::cout, formatting)
    {}

    JsonWriter::JsonWriter(const std::string& fileName,
                           JsonFormatting formatting)
        : JsonWriter(openFile(fileName), nullptr, formatting)
    {}

    JsonWriter::JsonWriter(std::ostream& stream, JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), &stream, formatting)
    {}

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream>&& streamPtr,
                           std::ostream* stream, JsonFormatting formatting)
        : m_StreamPtr(move(streamPtr)),
          m_Stream(m_StreamPtr ? m_StreamPtr.get() : stream),
          m_State(AT_START_OF_VALUE_NO_COMMA),
          m_Indentation(0),
          m_IndentationWidth(2),
          m_LanguagExtensions(0),
          m_FormattingEnabled(true),
          m_IndentationCharacter(' ')
    {
        formatting = formatting != JsonFormatting::DEFAULT
                     ? formatting : JsonFormatting::NONE;
        m_Contexts.push(Context('\0', formatting));
    }

    JsonWriter::JsonWriter(JsonWriter&& rhs)
        : m_StreamPtr(std::move(rhs.m_StreamPtr)),
          m_Contexts(std::move(rhs.m_Contexts)),
          m_Key(std::move(rhs.m_Key)),
          m_Indentation(rhs.m_Indentation),
          m_IndentationWidth(rhs.m_IndentationWidth),
          m_LanguagExtensions(rhs.m_LanguagExtensions),
          m_FormattingEnabled(rhs.m_FormattingEnabled),
          m_IndentationCharacter(rhs.m_IndentationCharacter)
    {
        m_Stream = m_StreamPtr.get();
        rhs.m_Stream = nullptr;
    }

    JsonWriter::~JsonWriter() = default;

    JsonWriter& JsonWriter::operator=(JsonWriter&& rhs)
    {
        m_StreamPtr = std::move(rhs.m_StreamPtr);
        m_Stream = rhs.m_Stream;
        rhs.m_Stream = nullptr;
        m_Contexts = std::move(rhs.m_Contexts);
        m_Key = std::move(rhs.m_Key);
        m_Indentation = rhs.m_Indentation;
        m_IndentationCharacter = rhs.m_IndentationCharacter;
        m_FormattingEnabled = rhs.m_FormattingEnabled;
        m_IndentationWidth = rhs.m_IndentationWidth;
        return *this;
    }

    std::ostream* JsonWriter::stream()
    {
        return m_Stream;
    }

    const std::string& JsonWriter::key() const
    {
        return m_Key;
    }

    JsonWriter& JsonWriter::key(const std::string& key)
    {
        m_Key = key;
        return *this;
    }

    JsonWriter& JsonWriter::beginArray()
    {
        return beginStructure('[', ']');
    }

    JsonWriter& JsonWriter::beginArray(const StructureParameters& parameters)
    {
        return beginStructure('[', ']', parameters.jsonParameters);
    }

    JsonWriter& JsonWriter::endArray()
    {
        return endStructure(']');
    }

    JsonWriter& JsonWriter::beginObject()
    {
      return beginStructure('{', '}');
    }

    JsonWriter& JsonWriter::beginObject(const StructureParameters& parameters)
    {
        return beginStructure('{', '}', parameters.jsonParameters);
    }

    JsonWriter& JsonWriter::endObject()
    {
        return endStructure('}');
    }

    JsonWriter& JsonWriter::null()
    {
        return rawValue("null");
    }

    JsonWriter& JsonWriter::boolean(bool value)
    {
        return rawValue(value ? "true" : "false");
    }

    JsonWriter& JsonWriter::value(char value)
    {
        return writeIntValueImpl(int16_t(value));
    }

    JsonWriter& JsonWriter::value(int8_t value)
    {
        return writeIntValueImpl(int16_t(value));
    }

    JsonWriter& JsonWriter::value(int16_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(int32_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(int64_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(uint8_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(uint16_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(uint32_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(uint64_t value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(float value)
    {
        return writeFloatValueImpl(value);
    }

    JsonWriter& JsonWriter::value(double value)
    {
        return writeFloatValueImpl(value);
    }

    JsonWriter& JsonWriter::value(long double value)
    {
        return writeFloatValueImpl(value);
    }

    JsonWriter& JsonWriter::value(const std::string& text)
    {
        beginValue();
        *m_Stream << "\"";
        if (!Ystring::hasUnescapedCharacters(text))
            *m_Stream << text;
        else
            *m_Stream << Ystring::escape(text);
        *m_Stream << "\"";
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::value(const std::wstring& text)
    {
        return value(Ystring::Conversion::convert<std::string>(
                text,
                Ystring::Encoding::UTF_16,
                Ystring::Encoding::UTF_8));
    }

    JsonWriter& JsonWriter::rawValue(const std::string& value)
    {
        beginValue();
        *m_Stream << value;
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::rawText(const std::string& value)
    {
        *m_Stream << value;
        return *this;
    }

    JsonWriter& JsonWriter::binary(const void* data, size_t size)
    {
        return base64(data, size);
    }

    JsonWriter& JsonWriter::base64(const void* data, size_t size)
    {
        return value(toBase64(data, size));
    }

    JsonWriter& JsonWriter::indent()
    {
        ++m_Indentation;
        return *this;
    }

    JsonWriter& JsonWriter::outdent()
    {
        if (m_Indentation == 0)
            YSON_THROW("Can't outdent, indentation level is 0.");
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
            if (formatting() == JsonFormatting::FORMAT)
                writeIndentationImpl();
            break;
        default:
            YSON_THROW("A comma has already been added.");
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
        default:
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

    bool JsonWriter::isFormattingEnabled() const
    {
        return m_FormattingEnabled;
    }

    JsonWriter& JsonWriter::setFormattingEnabled(bool value)
    {
        m_FormattingEnabled = value;
        return *this;
    }

    int JsonWriter::languageExtensions() const
    {
        return m_LanguagExtensions;
    }

    JsonWriter& JsonWriter::setLanguageExtensions(int value)
    {
        m_LanguagExtensions = value;
        return *this;
    }

    bool JsonWriter::isNonFiniteFloatsAsStringsEnabled() const
    {
        return languageExtension(NON_FINITE_FLOATS_AS_STRINGS);
    }

    JsonWriter& JsonWriter::setNonFiniteFloatsAsStringsEnabled(bool value)
    {
        return setLanguageExtension(NON_FINITE_FLOATS_AS_STRINGS, value);
    }

    bool JsonWriter::isUnquotedValueNamesEnabled() const
    {
        return languageExtension(UNQUOTED_VALUE_NAMES);
    }

    JsonWriter& JsonWriter::setUnquotedValueNamesEnabled(bool value)
    {
        return setLanguageExtension(UNQUOTED_VALUE_NAMES, value);
    }

    void JsonWriter::beginValue()
    {
        switch (m_State)
        {
        case AT_START_OF_STRUCTURE:
            if (formatting() == JsonFormatting::FORMAT)
            {
                *m_Stream << '\n';
                writeIndentationImpl();
            }
            break;
        case AT_END_OF_VALUE:
            switch (formatting())
            {
            case JsonFormatting::NONE:
                *m_Stream << ',';
                break;
            case JsonFormatting::FLAT:
                *m_Stream << ", ";
                break;
            case JsonFormatting::FORMAT:
                {
                    auto& c = m_Contexts.top();
                    if (c.parameters.valuesPerLine <= 1
                        || c.valueIndex >= c.parameters.valuesPerLine)
                    {
                        *m_Stream << ",\n";
                        writeIndentationImpl();
                    }
                    else
                    {
                        *m_Stream << ", ";
                    }
                }
                break;
            default:
                break;
            }
            break;
        case AT_START_OF_LINE_NO_COMMA:
        case AT_START_OF_LINE_AFTER_COMMA:
            if (formatting() == JsonFormatting::FORMAT)
                writeIndentationImpl();
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            switch (formatting())
            {
            case JsonFormatting::NONE:
                *m_Stream << ',';
                break;
            case JsonFormatting::FLAT:
                *m_Stream << ", ";
                break;
            case JsonFormatting::FORMAT:
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
            case JsonFormatting::FLAT:
                *m_Stream << ' ';
                break;
            case JsonFormatting::FORMAT:
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
        ++m_Contexts.top().valueIndex;
        if (isInsideObject())
        {
            if (!isUnquotedValueNamesEnabled()
                || !isJavaScriptIdentifier(m_Key))
                *m_Stream << "\"" << m_Key
                          << (formatting() != JsonFormatting::NONE ? "\": " : "\":");
            else
                *m_Stream << m_Key
                          << (formatting() != JsonFormatting::NONE ? ": " : ":");
        }
    }

    JsonFormatting JsonWriter::formatting() const
    {
        if (!m_FormattingEnabled)
            return JsonFormatting::NONE;
        if (m_Contexts.empty())
            return JsonFormatting::FORMAT;
        return m_Contexts.top().parameters.formatting;
    }

    bool JsonWriter::isInsideObject() const
    {
        return !m_Contexts.empty() && m_Contexts.top().endChar == '}';
    }

    JsonWriter& JsonWriter::beginStructure(char startChar, char endChar,
                                           JsonParameters parameters)
    {
        beginValue();
        *m_Stream << startChar;
        if (parameters.formatting == JsonFormatting::DEFAULT)
        {
            parameters.formatting = m_Contexts.top().parameters.formatting;
            if (m_Contexts.top().parameters.valuesPerLine > 1)
                parameters.formatting = JsonFormatting::FLAT;
        }
        m_Contexts.push(Context(endChar, parameters));
        if (formatting() == JsonFormatting::FORMAT)
            indent();
        m_State = AT_START_OF_STRUCTURE;
        return *this;
    }

    JsonWriter& JsonWriter::endStructure(char endChar)
    {
        if (m_Contexts.empty() || m_Contexts.top().endChar != endChar)
        {
            // This function could be called from the destructor of some
            // RAII-class. If so, make sure that we're not already processing
            // another exception before throwing our own.
            if (!std::uncaught_exception())
                YSON_THROW(std::string("Incorrect position for '")
                           + endChar + "'");
            else
                return *this;
        }

        switch (m_State)
        {
        case AT_START_OF_VALUE_NO_COMMA:
        case AT_START_OF_STRUCTURE:
            if (formatting() == JsonFormatting::FORMAT)
                outdent();
            m_Contexts.pop();
            break;
        case AT_END_OF_VALUE:
            if (formatting() == JsonFormatting::FORMAT)
            {
                outdent();
                m_Contexts.pop();
                if (m_Contexts.top().parameters.valuesPerLine <= 1)
                {
                    *m_Stream << '\n';
                    writeIndentationImpl();
                }
            }
            else
            {
                m_Contexts.pop();
            }
            break;
        case AT_START_OF_LINE_NO_COMMA:
        case AT_START_OF_LINE_BEFORE_COMMA:
            if (formatting() == JsonFormatting::FORMAT)
            {
                outdent();
                writeIndentationImpl();
                m_Contexts.pop();
            }
            break;
        default:
            YSON_THROW(std::string("Incorrect position for '")
                       + endChar + "'");
        }

        *m_Stream << endChar;
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    void JsonWriter::writeIndentationImpl()
    {
        auto n = m_Indentation * m_IndentationWidth;
        for (size_t i = 0; i < n; ++i)
            *m_Stream << m_IndentationCharacter;
        m_Contexts.top().valueIndex = 0;
    }

    template <typename T>
    JsonWriter& JsonWriter::writeIntValueImpl(T number)
    {
        beginValue();
        *m_Stream << number;
        m_State = AT_END_OF_VALUE;
        return *this;
    }

    template <typename T>
    JsonWriter& JsonWriter::writeFloatValueImpl(T number)
    {
        if (std::isfinite(number))
        {
            beginValue();
            *m_Stream << number;
            m_State = AT_END_OF_VALUE;
        }
        else if (!languageExtension(NON_FINITE_FLOATS_AS_STRINGS))
        {
            YSON_THROW(std::string("Illegal floating point value '")
                       + std::to_string(number) + "'");
        }
        else
        {
            if (std::isnan(number))
                value("NaN");
            else if (number < 0)
                value("-infinity");
            else
                value("infinity");
        }
        return *this;
    }

    bool JsonWriter::languageExtension(
            JsonWriter::LanguageExtensions ext) const
    {
        return (m_LanguagExtensions & ext) != 0;
    }

    JsonWriter& JsonWriter::setLanguageExtension(
            JsonWriter::LanguageExtensions ext,
            bool value)
    {
        if (value)
            m_LanguagExtensions |= ext;
        else
            m_LanguagExtensions &= ~ext;
        return *this;
    }
}
