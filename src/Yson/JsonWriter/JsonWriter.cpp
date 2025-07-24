//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
// ReSharper disable CppParameterMayBeConst
#include "Yson/JsonWriter.hpp"

#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <stack>
#include <Yconvert/Convert.hpp>
#include "Yson/YsonException.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/Escape.hpp"
#include "Yson/Common/IsJavaScriptIdentifier.hpp"
#include "JsonWriterUtilities.hpp"

namespace Yson
{
    namespace
    {
        constexpr size_t MAX_BUFFER_SIZE = 64 * 1024;
        const std::string EMPTY_STRING;

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
        std::vector<char> sprintfBuffer = std::vector<char>(32);
        std::string buffer;
        size_t maxBufferSize = MAX_BUFFER_SIZE;
        State state = AT_START_OF_VALUE_NO_COMMA;
        unsigned indentationWidth = 2;
        int languageExtensions = 0;
        int floatingPointPrecision = 9;
        bool formattingEnabled = true;
        char indentationCharacter = ' ';
        int maximumLineWidth = 120;
        std::unique_ptr<Yconvert::Converter> wstringConverter;
    };

    JsonWriter::JsonWriter(JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), nullptr, formatting)
    {}

    JsonWriter::JsonWriter(const std::filesystem::path& fileName,
                           JsonFormatting formatting)
        : JsonWriter(std::make_unique<std::ofstream>(fileName),
                     nullptr,
                     formatting)
    {}

    JsonWriter::JsonWriter(std::ostream& stream, JsonFormatting formatting)
        : JsonWriter(std::unique_ptr<std::ostream>(), &stream, formatting)
    {}

    JsonWriter::JsonWriter(std::unique_ptr<std::ostream> streamPtr,
                           std::ostream* stream,
                           JsonFormatting formatting)
        : m_Members(std::make_unique<Members>())
    {
        m_Members->streamPtr = std::move(streamPtr);
        m_Members->stream = m_Members->streamPtr
                                ? m_Members->streamPtr.get()
                                : stream;
        formatting = formatting != JsonFormatting::DEFAULT
                         ? formatting
                         : JsonFormatting::NONE;
        m_Members->contexts.emplace('\0', JsonParameters(formatting));
        m_Members->buffer.reserve(MAX_BUFFER_SIZE);
        if (!m_Members->stream)
            m_Members->maxBufferSize = SIZE_MAX;
    }

    JsonWriter::JsonWriter(JsonWriter&& rhs) noexcept
        : m_Members(std::move(rhs.m_Members))
    {}

    JsonWriter::~JsonWriter()
    {
        if (m_Members)
            JsonWriter::flush();
    }

    JsonWriter& JsonWriter::operator=(JsonWriter&& rhs) noexcept
    {
        m_Members = std::move(rhs.m_Members);
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

    const std::string& JsonWriter::str() const
    {
        auto& m = members();
        if (m.stream)
            return EMPTY_STRING;
        return m.buffer;
    }

    const std::string& JsonWriter::key() const
    {
        return members().key;
    }

    JsonWriter& JsonWriter::key(std::string key)
    {
        if (!hasUnescapedCharacters(key))
            members().key = std::move(key);
        else
            members().key = escape(key, isEscapeNonAsciiCharactersEnabled());

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
        return writeIntValueImpl(int(value));
    }

    JsonWriter& JsonWriter::value(signed char value)
    {
        return writeIntValueImpl(int(value));
    }

    JsonWriter& JsonWriter::value(short value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(int value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(long value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(long long value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(unsigned char value)
    {
        return writeIntValueImpl(unsigned(value));
    }

    JsonWriter& JsonWriter::value(unsigned short value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(unsigned value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(unsigned long value)
    {
        return writeIntValueImpl(value);
    }

    JsonWriter& JsonWriter::value(unsigned long long value)
    {
        return writeIntValueImpl(value);
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

    JsonWriter& JsonWriter::value(std::string_view value)
    {
        if (!hasUnescapedCharacters(value))
            return writeString(value);

        return writeString(escape(
            value, isEscapeNonAsciiCharactersEnabled()));
    }

    JsonWriter& JsonWriter::value(std::wstring_view value)
    {
        auto& converter = members().wstringConverter;
        if (!converter)
        {
            converter = std::make_unique<Yconvert::Converter>(
                Yconvert::Encoding::WSTRING_NATIVE,
                Yconvert::Encoding::UTF_8);
            converter->set_error_policy(Yconvert::ErrorPolicy::REPLACE);
        }

        return JsonWriter::value(Yconvert::convert_to<std::string>(
            value, *converter));
    }

    JsonWriter& JsonWriter::rawValue(std::string_view value)
    {
        beginValue();
        write(value);
        members().state = AT_END_OF_VALUE;
        return *this;
    }

    JsonWriter& JsonWriter::rawText(std::string_view value)
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
        const auto newSize = m.indentation.size() - m.indentationWidth;
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

    JsonWriter& JsonWriter::setIndentation(char character, unsigned width)
    {
        auto& m = members();
        m.indentationCharacter = character;
        m.indentationWidth = width;
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
        return members().languageExtensions;
    }

    JsonWriter& JsonWriter::setLanguageExtensions(int value)
    {
        members().languageExtensions = value;
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

    bool JsonWriter::isQuotedNonFiniteFloatsEnabled() const
    {
        return languageExtension(QUOTED_NON_FINITE_FLOATS);
    }

    JsonWriter& JsonWriter::setQuotedNonFiniteFloatsEnabled(bool value)
    {
        setNonFiniteFloatsEnabled(true);
        return setLanguageExtension(QUOTED_NON_FINITE_FLOATS, value);
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
            m.stream->write(m.buffer.data(),
                            std::streamsize(m.buffer.size()));
            m.buffer.clear();
        }
        return *this;
    }

    void JsonWriter::write(std::string_view s)
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
            m.stream->write(s, std::streamsize(size));
        }
    }

    void JsonWriter::writeMultiLine(std::string_view s)
    {
        const auto maxWidth = size_t(m_Members->maximumLineWidth);
        if (s.size() <= maxWidth / 2)
        {
            write(s.data(), s.size());
        }
        else
        {
            size_t from = 0;
            const auto n = maxWidth - getCurrentLineWidth(m_Members->buffer,
                                                          maxWidth);
            if (n > 2)
            {
                const auto to = findSplitPos(s, n - 1);
                write(s.data(), to);
                from = to;
            }
            while (true)
            {
                const auto to = findSplitPos(s, from + maxWidth - 1);
                if (from == to)
                    break;
                m_Members->buffer.append("\\\n");
                write(s.data() + from, to - from);
                from = to;
            }
        }
    }

    JsonWriter& JsonWriter::writeString(std::string_view text)
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
        YSON_THROW("The members of this JsonWriter instance have"
            " been moved to another instance.");
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
                m.buffer.append(", ");
                break;
            case JsonFormatting::FORMAT:
                {
                    const auto& c = m.contexts.top();
                    if (c.parameters.valuesPerLine <= 1
                        || c.valueIndex >= c.parameters.valuesPerLine)
                    {
                        m.buffer.append(",\n");
                        writeIndentationImpl();
                    }
                    else
                    {
                        m.buffer.append(", ");
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
                parameters.formatting = std::min(parameters.formatting, JsonFormatting::FLAT);
        }
        else if (m.contexts.top().parameters.formatting < parameters.formatting)
        {
            parameters.formatting = m.contexts.top().parameters.formatting;
        }

        m.contexts.emplace(endChar, parameters);
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
                return *this;

            YSON_THROW(std::string("Incorrect position for '")
                + endChar + "'");
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
    JsonWriter& JsonWriter::writeIntValueImpl(const T number)
    {
        beginValue();
        auto& m = members();
        auto& buffer = m.sprintfBuffer;
        auto result = std::to_chars(buffer.data(),
                                    buffer.data() + buffer.size(), number);
        while (result.ec != std::errc())
        {
            buffer.resize(buffer.size() * 2);
            result = std::to_chars(buffer.data(),
                                   buffer.data() + buffer.size(), number);
        }
        write(buffer.data(), size_t(result.ptr - buffer.data()));
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
            auto& buffer = m.sprintfBuffer;
            const auto precision = std::min(m.floatingPointPrecision,
                                            std::numeric_limits<T>::digits10);
#ifdef YSON_USE_TO_CHARS_FOR_FLOATS
            auto result = std::to_chars(buffer.data(),
                                        buffer.data() + buffer.size(),
                                        number,
                                        std::chars_format::general,
                                        precision);

            while (result.ec != std::errc())
            {
                buffer.resize(buffer.size() * 2);
                result = std::to_chars(buffer.data(),
                                       buffer.data() + buffer.size(),
                                       number,
                                       std::chars_format::general,
                                       precision);
            }
            write(buffer.data(), size_t(result.ptr - buffer.data()));
#else
            auto size = snprintf(buffer.data(), buffer.size(),
                                 format, precision, number);
            if (size > buffer.size())
            {
                buffer.resize(size + 1);
                size = snprintf(buffer.data(), buffer.size(),
                                format, precision, number);
            }
            write(buffer.data(), size);
#endif
            m.state = AT_END_OF_VALUE;
        }
        else if (!languageExtension(NON_FINITE_FLOATS))
        {
            YSON_THROW(std::string("Illegal floating point value '")
                + std::to_string(number) + "'");
        }
        else if (!languageExtension(QUOTED_NON_FINITE_FLOATS))
        {
            if (std::isnan(number))
                rawValue("NaN");
            else if (number < 0)
                rawValue("-Infinity");
            else
                rawValue("Infinity");
        }
        else
        {
            if (std::isnan(number))
                value("NaN");
            else if (number < 0)
                value("-Infinity");
            else
                value("Infinity");
        }
        return *this;
    }

    bool JsonWriter::languageExtension(LanguageExtensions ext) const
    {
        return (members().languageExtensions & ext) != 0;
    }

    JsonWriter& JsonWriter::setLanguageExtension(LanguageExtensions ext,
                                                 bool value)
    {
        if (value)
            members().languageExtensions |= ext;
        else
            members().languageExtensions &= ~ext;
        return *this;
    }
}
