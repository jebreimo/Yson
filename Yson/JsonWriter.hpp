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
        void setIndentation(const std::string& indentation);

        const std::string& valueName() const;
        void setValueName(const std::string& name);

        void beginArray(Formatting formatting = DEFAULT);
        void beginArray(const std::string& name, Formatting formatting = DEFAULT);
        void endArray();

        void beginObject(Formatting formatting = DEFAULT);
        void beginObject(const std::string& name, Formatting formatting = DEFAULT);
        void endObject();

        void newline(bool comma = false);

        void value();
        void value(int32_t value);
        void value(int64_t value);
        void value(uint32_t value);
        void value(uint64_t value);
        void value(double value);
        void value(const char* value);
        void value(const std::string& value);
        void value(const wchar_t* value);
        void value(const std::wstring& value);
        void value(bool value);
        void value(const JsonValue& jsonValue, Formatting formatting = DEFAULT);

        template <typename T>
        void value(const std::string& name, T value)
        {
            setValueName(name);
            this->value(value);
        }

        void value(const std::string& name, const JsonValue& value, Formatting formatting = DEFAULT);
    private:
        void beginValue();
        void endStructure(char endChar);
        void indent();
        bool shouldFormat(Formatting formatting);
        template <typename T>
        void writeValue(T value);

        struct Context
        {
            Context() : endChar(0), format(false) {}
            Context(char endChar, bool format) : endChar(endChar), format(format) {}
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
