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

        const std::string& indentation() const;

        JsonWriter& setIndentation(const std::string& indentation);

        const std::string& valueName() const;

        JsonWriter& setValueName(const std::string& name);

        JsonWriter& beginArray(Formatting formatting = DEFAULT);

        JsonWriter& beginArray(const std::string& name,
                               Formatting formatting = DEFAULT);

        JsonWriter& endArray();

        JsonWriter& beginObject(Formatting formatting = DEFAULT);

        JsonWriter& beginObject(const std::string& name,
                                Formatting formatting = DEFAULT);

        JsonWriter& endObject();

        JsonWriter& newline(bool comma = false);

        JsonWriter& value();

        JsonWriter& value(int32_t value);
        JsonWriter& value(int64_t value);
        JsonWriter& value(uint32_t value);
        JsonWriter& value(uint64_t value);
        JsonWriter& value(double value);
        JsonWriter& value(const char* value);
        JsonWriter& value(const std::string& value);
        JsonWriter& value(const wchar_t* value);
        JsonWriter& value(const std::wstring& value);
        JsonWriter& value(bool value);

        template <typename T>
        JsonWriter& value(const std::string& name, T value)
        {
            setValueName(name);
            this->value(value);
        }
    private:
        void beginValue();
        void endStructure(char endChar);
        void indent();
        bool shouldFormat(Formatting formatting);
        template <typename T>
        void writeValue(T value);

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
