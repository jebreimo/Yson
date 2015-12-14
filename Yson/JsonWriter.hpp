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

namespace Yson
{
    class JsonValue;

    class JsonWriter
    {
    public:
        enum Formatting {DEFAULT, FORMATTED, PLAIN};

        JsonWriter();
        JsonWriter(std::ostream& stream);
        JsonWriter(std::unique_ptr<std::ostream> stream);
        JsonWriter(JsonWriter&& rhs);
        ~JsonWriter();

        JsonWriter& operator=(JsonWriter&& rhs);

        std::ostream* stream();

        const std::string& indentation() const;

        JsonWriter& setIndentation(const std::string& indentation);

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

        JsonWriter& writeNewline(bool comma = false);

        JsonWriter& writeNull();

        JsonWriter& writeValue(bool value);

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

        JsonWriter& writeValue(const char* value);

        JsonWriter& writeValue(const std::string& value);

        JsonWriter& writeValue(const wchar_t* value);

        JsonWriter& writeValue(const std::wstring& value);

        template <typename T>
        JsonWriter& writeValue(const std::string& name, const T& value)
        {
            setValueName(name);
            this->writeValue(value);
            return *this;
        }
    private:
        void beginValue();
        void writeEndStructure(char endChar);
        void writeIndent();
        bool shouldFormat(Formatting formatting);
        template <typename T>
        JsonWriter& writeValueImpl(T value);

        struct Context
        {
            Context()
                : endChar(0),
                  format(false)
            {}

            Context(char endChar, bool format)
                : endChar(endChar),
                  format(format)
            {}

            char endChar;
            bool format;
        };

        std::stack<Context> m_Context;
        std::string m_ValueName;
        bool m_IsFirst;
        std::string m_Indentation;
        std::ostream* m_Stream;
        std::unique_ptr<std::ostream> m_StreamPtr;
    };
}
