//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonWriter.hpp"

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stack>
#include <Ystring/Conversion.hpp>
#include <Ystring/Escape/Escape.hpp>
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/GetUnicodeFileName.hpp"
#include "Yson/Common/ThrowYsonException.hpp"
#include "Yson/Common/IsJavaScriptIdentifier.hpp"
#include "JsonWriterUtilities.hpp"

namespace Yson
{
    namespace
    {
        constexpr size_t MAX_BUFFER_SIZE = 64 * 1024;

        struct Context
        {
            Context() = default;

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
    }

    struct JsonWriter::Members
    {
        std::unique_ptr<std::ostream> streamPtr;
        std::ostream* stream = nullptr;
        std::stack<Context> contexts;
        std::string indentation;
        std::string key;
        std::array<char, 256> sprintfBuffer;
        std::vector<char> buffer;
        size_t maxBufferSize = MAX_BUFFER_SIZE;
        State state = AT_START_OF_VALUE_NO_COMMA;
        unsigned indentationWidth = 2;
        int languagExtensions = 0;
        int floatingPointPrecision = 9;
        bool formattingEnabled = true;
        char indentationCharacter = ' ';
        int maximumLineWidth = 120;
    };

    JsonWriter::JsonWriter(JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), nullptr, formatting)
    {}

    JsonWriter::JsonWriter(const std::string& fileName,
                           JsonFormatting formatting)
        : JsonWriter(std::make_unique<std::ofstream>(getUnicodeFileName(fileName)),
                     nullptr,
                     formatting)
    {}

    JsonWriter::JsonWriter(std::ostream& stream, JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), &stream, formatting)
    {}

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream> streamPtr,
                           std::ostream* stream,
                           JsonFormatting formatting)
        : m_Members(new Members)
    {
        m_Members->streamPtr = move(streamPtr);
        m_Members->stream = m_Members->streamPtr
                            ? m_Members->streamPtr.get()
                            : stream;
        formatting = formatting != JsonFormatting::DEFAULT
                     ? formatting
                     : JsonFormatting::NONE;
        m_Members->contexts.push(Context('\0', JsonParameters(formatting)));
        m_Members->buffer.reserve(MAX_BUFFER_SIZE);
        if (!m_Members->stream)
            m_Members->maxBufferSize = SIZE_MAX;
    }

    JsonWriter::JsonWriter(JsonWriter&& rhs) noexcept
        : m_Members(move(rhs.m_Members))
    {}

    JsonWriter::~JsonWriter()
    {
        if (m_Members)
            JsonWriter::flush();
    }

    JsonWriter& JsonWriter::operator=(JsonWriter&& rhs) noexcept
    {
        m_Members = move(rhs.m_Members);
        return *this;
    }

    std::ostream* JsonWriter::stream()
    {
        flush();
        return members().stream;
    }

    std::pair<const void*, size_t> JsonWriter::buffer() const
    {
        auto& m = members();
        if (m.stream)
            return {nullptr, 0};
        return {m.buffer.data(), m.buffer.size()};
    }

    const std::string& JsonWriter::key() const
    {
        return members().key;
    }

    JsonWriter& JsonWriter::key(const std::string& key)
    {
        members().key = key;
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
        return writeIntValueImpl(value, "%hhi");
    }

    JsonWriter& JsonWriter::value(int8_t value)
    {
        return writeIntValueImpl(value, "%hhi");
    }

    JsonWriter& JsonWriter::value(int16_t value)
    {
        return writeIntValueImpl(value, "%hi");
    }

    JsonWriter& JsonWriter::value(int32_t value)
    {
        return writeIntValueImpl(value, "%i");
    }

    JsonWriter& JsonWriter::value(int64_t value)
    {
        return writeIntValueImpl(value, "%lli");
    }

    JsonWriter& JsonWriter::value(uint8_t value)
    {
        return writeIntValueImpl(value, "%hhu");
    }

    JsonWriter& JsonWriter::value(uint16_t value)
    {
        return writeIntValueImpl(value, "%hu");
    }

    JsonWriter& JsonWriter::value(uint32_t value)
    {
        return writeIntValueImpl(value, "%u");
    }

    JsonWriter& JsonWriter::value(uint64_t value)
    {
        return writeIntValueImpl(value, "%llu");
    }

    JsonWriter& JsonWriter::value(float value)
    {
        return writeFloatValueImpl(value, "%.*g");
    }

    JsonWriter& JsonWriter::value(double value)
    {
        return writeFloatValueImpl(value, "%.*g");
    }

    JsonWriter& JsonWriter::value(long double value)
    {
        return writeFloatValueImpl(value, "%.*Lg");
    }

    JsonWriter& JsonWriter::value(const std::string& text)
    {
        if (!Ystring::hasUnescapedCharacters(text))
        {
            return writeString(text);
        }
        else
        {
            return writeString(Ystring::escape(
                    text, isEscapeNonAsciiCharactersEnabled()));
        }
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
        write(value);
        members().state = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::rawText(const std::string& value)
    {
        write(value);
        return *this;
    }

    JsonWriter& JsonWriter::binary(const void* data, size_t size)
    {
        return base64(data, size);
    }

    JsonWriter& JsonWriter::base64(const void* data, size_t size)
    {
        return writeString(toBase64(data, size));
    }

    JsonWriter& JsonWriter::indent()
    {
        auto& m = members();
        m.indentation.append(m.indentationWidth, m.indentationCharacter);
        return *this;
    }

    JsonWriter& JsonWriter::outdent()
    {
        auto& m = members();
        if (m.indentation.size() < m.indentationWidth)
            YSON_THROW("Can't outdent, indentation level is 0.");
        auto newSize = m.indentation.size() - m.indentationWidth;
        m.indentation.resize(newSize);
        return *this;
    }

    JsonWriter& JsonWriter::writeComma()
    {
        auto& m = members();
        switch (m.state)
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
        m.buffer.push_back(',');
        m.state = AFTER_COMMA;
        return *this;
    }

    JsonWriter& JsonWriter::writeIndentation()
    {
        auto& m = members();
        switch (m.state)
        {
        case AT_START_OF_LINE_NO_COMMA:
            writeIndentationImpl();
            m.state = AT_START_OF_VALUE_NO_COMMA;
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            writeIndentationImpl();
            m.state = AT_END_OF_VALUE;
            break;
        case AT_START_OF_LINE_AFTER_COMMA:
            writeIndentationImpl();
            m.state = AT_START_OF_VALUE_AFTER_COMMA;
            break;
        default:
            break;
        }
        return *this;
    }

    JsonWriter& JsonWriter::writeNewline()
    {
        auto& m = members();
        m.buffer.push_back('\n');
        switch (m.state)
        {
        case AT_START_OF_VALUE_NO_COMMA:
        case AT_START_OF_STRUCTURE:
            m.state = AT_START_OF_LINE_NO_COMMA;
            break;
        case AT_START_OF_VALUE_AFTER_COMMA:
        case AFTER_COMMA:
            m.state = AT_START_OF_LINE_AFTER_COMMA;
            break;
        case AT_END_OF_VALUE:
            m.state = AT_START_OF_LINE_BEFORE_COMMA;
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

        auto& m = members();
        m.buffer.resize(m.buffer.size() + count, ' ');
        switch (m.state)
        {
        case AT_START_OF_LINE_NO_COMMA:
            m.state = AT_START_OF_VALUE_NO_COMMA;
            break;
        case AT_START_OF_LINE_BEFORE_COMMA:
            m.state = AT_END_OF_VALUE;
            break;
        case AT_START_OF_LINE_AFTER_COMMA:
        case AFTER_COMMA:
            m.state = AT_START_OF_VALUE_AFTER_COMMA;
            break;
        default:
            break;
        }
        return *this;
    }

    std::pair<char, unsigned> JsonWriter::indentation() const
    {
        auto& m = members();
        return std::make_pair(m.indentationCharacter, m.indentationWidth);
    }

    JsonWriter& JsonWriter::setIndentation(char character, unsigned count)
    {
        auto& m = members();
        m.indentationCharacter = character;
        m.indentationWidth = count;
        return *this;
    }

    bool JsonWriter::isFormattingEnabled() const
    {
        return members().formattingEnabled;
    }

    JsonWriter& JsonWriter::setFormattingEnabled(bool value)
    {
        members().formattingEnabled = value;
        return *this;
    }

    int JsonWriter::languageExtensions() const
    {
        return members().languagExtensions;
    }

    JsonWriter& JsonWriter::setLanguageExtensions(int value)
    {
        members().languagExtensions = value;
        return *this;
    }

    bool JsonWriter::isNonFiniteFloatsEnabled() const
    {
        return languageExtension(NON_FINITE_FLOATS);
    }

    JsonWriter& JsonWriter::setNonFiniteFloatsEnabled(bool value)
    {
        return setLanguageExtension(NON_FINITE_FLOATS, value);
    }

    bool JsonWriter::isUnquotedValueNamesEnabled() const
    {
        return languageExtension(UNQUOTED_VALUE_NAMES);
    }

    JsonWriter& JsonWriter::setUnquotedValueNamesEnabled(bool value)
    {
        return setLanguageExtension(UNQUOTED_VALUE_NAMES, value);
    }

    bool JsonWriter::isMultilineStringsEnabled() const
    {
        return languageExtension(MULTILINE_STRINGS);
    }

    JsonWriter& JsonWriter::setMultilineStringsEnabled(bool value)
    {
        return setLanguageExtension(MULTILINE_STRINGS, value);
    }

    bool JsonWriter::isEscapeNonAsciiCharactersEnabled() const
    {
        return languageExtension(ESCAPE_NON_ASCII_CHARACTERS);
    }

    JsonWriter& JsonWriter::setEscapeNonAsciiCharactersEnabled(bool value)
    {
        return setLanguageExtension(ESCAPE_NON_ASCII_CHARACTERS, value);
    }

    int JsonWriter::maximumLineWidth() const
    {
        return m_Members->maximumLineWidth;
    }

    JsonWriter& JsonWriter::setMaximumLineWidth(int value)
    {
        if (value < 8)
            YSON_THROW("Maximum line width can not be less than 8.");
        m_Members->maximumLineWidth = value;
        return *this;
    }

    int JsonWriter::floatingPointPrecision() const
    {
        return members().floatingPointPrecision;
    }

    JsonWriter& JsonWriter::setFloatingPointPrecision(int value)
    {
        members().floatingPointPrecision = std::max(value, 1);
        return *this;
    }

    JsonWriter& JsonWriter::flush()
    {
        auto& m = members();
        if (m.stream && !m.buffer.empty())
        {
            m.stream->write(m.buffer.data(), m.buffer.size());
            m.buffer.clear();
        }
        return *this;
    }

    void JsonWriter::write(const std::string& s)
    {
        write(s.data(), s.size());
    }

    void JsonWriter::write(const char* s, size_t size)
    {
        auto& m = members();
        if (m.buffer.size() + size <= m.maxBufferSize)
        {
            m.buffer.insert(m.buffer.end(), s, s + size);
        }
        else
        {
            flush();
            m.stream->write(s, size);
        }
    }

    void JsonWriter::writeMultiLine(const std::string& s)
    {
        auto maxWidth = size_t(m_Members->maximumLineWidth);
        if (s.size() <= maxWidth / 2)
        {
            write(s.data(), s.size());
        }
        else
        {
            auto n = maxWidth - getCurrentLineWidth(m_Members->buffer,
                                                    maxWidth);
            size_t from = 0;
            if (n > 2)
            {
                auto to = findSplitPos(s, n - 1);
                write(s.data(), to);
                from = to;
            }
            while (true)
            {
                auto to = findSplitPos(s, from + maxWidth - 1);
                if (from == to)
                    break;
                m_Members->buffer.push_back('\\');
                m_Members->buffer.push_back('\n');
                write(s.data() + from, to - from);
                from = to;
            }
        }
    }

    JsonWriter& JsonWriter::writeString(const std::string& text)
    {
        beginValue();
        auto& m = members();
        m.buffer.push_back('"');
        if (!languageExtension(MULTILINE_STRINGS))
            write(text);
        else
            writeMultiLine(text);
        m.buffer.push_back('"');
        m.state = AT_END_OF_VALUE;
        return *this;

    }

    JsonWriter::Members& JsonWriter::members() const
    {
        if (m_Members)
            return *m_Members;
        YSON_THROW("The members of this JsonWriter instance have been moved to another instance.");
    }

    void JsonWriter::beginValue()
    {
        auto& m = members();
        switch (m.state)
        {
        case AT_START_OF_STRUCTURE:
            if (formatting() == JsonFormatting::FORMAT)
            {
                m.buffer.push_back('\n');
                writeIndentationImpl();
            }
            break;
        case AT_END_OF_VALUE:
            switch (formatting())
            {
            case JsonFormatting::NONE:
                m.buffer.push_back(',');
                break;
            case JsonFormatting::FLAT:
                m.buffer.push_back(',');
                m.buffer.push_back(' ');
                break;
            case JsonFormatting::FORMAT:
                {
                    auto& c = m.contexts.top();
                    if (c.parameters.valuesPerLine <= 1
                        || c.valueIndex >= c.parameters.valuesPerLine)
                    {
                        m.buffer.push_back(',');
                        m.buffer.push_back('\n');
                        writeIndentationImpl();
                    }
                    else
                    {
                        m.buffer.push_back(',');
                        m.buffer.push_back(' ');
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
                m.buffer.push_back(',');
                break;
            case JsonFormatting::FLAT:
                m.buffer.push_back(',');
                m.buffer.push_back(' ');
                break;
            case JsonFormatting::FORMAT:
                writeIndentationImpl();
                m.buffer.push_back(',');
                m.buffer.push_back('\n');
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
                m.buffer.push_back(' ');
                break;
            case JsonFormatting::FORMAT:
                m.buffer.push_back('\n');
                writeIndentationImpl();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        ++m.contexts.top().valueIndex;
        if (isInsideObject())
        {
            if (!isUnquotedValueNamesEnabled()
                || !isJavaScriptIdentifier(m.key))
            {
                m.buffer.push_back('"');
                write(m.key);
                m.buffer.push_back('"');
                m.buffer.push_back(':');
                if (formatting() != JsonFormatting::NONE)
                    m.buffer.push_back(' ');
            }
            else
            {
                write(m.key);
                m.buffer.push_back(':');
                if (formatting() != JsonFormatting::NONE)
                    m.buffer.push_back(' ');
            }
        }
    }

    JsonFormatting JsonWriter::formatting() const
    {
        auto& m = members();
        if (!m.formattingEnabled)
            return JsonFormatting::NONE;
        if (m.contexts.empty())
            return JsonFormatting::FORMAT;
        return m.contexts.top().parameters.formatting;
    }

    bool JsonWriter::isInsideObject() const
    {
        auto& m = members();
        return !m.contexts.empty() && m.contexts.top().endChar == '}';
    }

    JsonWriter& JsonWriter::beginStructure(char startChar, char endChar,
                                           JsonParameters parameters)
    {
        beginValue();
        auto& m = members();
        m.buffer.push_back(startChar);
        if (parameters.formatting == JsonFormatting::DEFAULT)
        {
            parameters.formatting = m.contexts.top().parameters.formatting;
            if (m.contexts.top().parameters.valuesPerLine > 1)
                parameters.formatting = JsonFormatting::FLAT;
        }
        m.contexts.push(Context(endChar, parameters));
        if (formatting() == JsonFormatting::FORMAT)
            indent();
        m.state = AT_START_OF_STRUCTURE;
        return *this;
    }

    JsonWriter& JsonWriter::endStructure(char endChar)
    {
        auto& m = members();
        if (m.contexts.empty() || m.contexts.top().endChar != endChar)
        {
            // This function could be called from the destructor of some
            // RAII-class. If so, make sure that we're not already processing
            // another exception before throwing our own.
            if (std::uncaught_exceptions())
                YSON_THROW(std::string("Incorrect position for '")
                           + endChar + "'");
            else
                return *this;
        }

        switch (m.state)
        {
        case AT_START_OF_VALUE_NO_COMMA:
        case AT_START_OF_STRUCTURE:
            if (formatting() == JsonFormatting::FORMAT)
                outdent();
            m.contexts.pop();
            break;
        case AT_END_OF_VALUE:
            if (formatting() == JsonFormatting::FORMAT)
            {
                outdent();
                m.contexts.pop();
                if (m.contexts.top().parameters.valuesPerLine <= 1)
                {
                    m.buffer.push_back('\n');
                    writeIndentationImpl();
                }
            }
            else
            {
                m.contexts.pop();
            }
            break;
        case AT_START_OF_LINE_NO_COMMA:
        case AT_START_OF_LINE_BEFORE_COMMA:
            if (formatting() == JsonFormatting::FORMAT)
            {
                outdent();
                writeIndentationImpl();
                m.contexts.pop();
            }
            break;
        default:
            YSON_THROW(std::string("Incorrect position for '")
                       + endChar + "'");
        }

        m.buffer.push_back(endChar);
        m.state = AT_END_OF_VALUE;
        return *this;
    }

    void JsonWriter::writeIndentationImpl()
    {
        auto& m = members();
        write(m.indentation);
        m.contexts.top().valueIndex = 0;
    }

    template <typename T>
    JsonWriter& JsonWriter::writeIntValueImpl(T number, const char* format)
    {
        beginValue();
        auto& m = members();
        auto size = sprintf(m.sprintfBuffer.data(), format, number);
        write(m.sprintfBuffer.data(), size);
        m.state = AT_END_OF_VALUE;
        return *this;
    }

    template <typename T>
    JsonWriter& JsonWriter::writeFloatValueImpl(T number, const char* format)
    {
        if (std::isfinite(number))
        {
            beginValue();
            auto& m = members();
            auto size = sprintf(m.sprintfBuffer.data(),
                                format,
                                std::min(m.floatingPointPrecision,
                                         std::numeric_limits<T>::digits10),
                                number);
            write(m.sprintfBuffer.data(), size);
            m.state = AT_END_OF_VALUE;
        }
        else if (!languageExtension(NON_FINITE_FLOATS))
        {
            YSON_THROW(std::string("Illegal floating point value '")
                       + std::to_string(number) + "'");
        }
        else
        {
            if (std::isnan(number))
                rawValue("NaN");
            else if (number < 0)
                rawValue("-Infinity");
            else
                rawValue("Infinity");
        }
        return *this;
    }

    bool JsonWriter::languageExtension(LanguageExtensions ext) const
    {
        return (members().languagExtensions & ext) != 0;
    }

    JsonWriter& JsonWriter::setLanguageExtension(LanguageExtensions ext,
                                                 bool value)
    {
        if (value)
            members().languagExtensions |= ext;
        else
            members().languagExtensions &= ~ext;
        return *this;
    }
}
