//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-25
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <istream>
#include <stack>
#include <string>
#include <Ystring/Encoding.hpp>
#include "JsonTokenizer.hpp"
#include "JsonValue.hpp"
#include "TextReader.hpp"
#include "ValueType.hpp"

namespace Yson {

    class JsonReader
    {
    public:
        JsonReader(std::istream& stream,
                   Ystring::Encoding_t encoding = Ystring::Encoding::UNKNOWN);

        JsonReader(const std::string& fileName,
                   Ystring::Encoding_t encoding = Ystring::Encoding::UNKNOWN);

        bool nextKey();

        bool nextValue();

        bool nextToken();

        ValueType_t valueType() const;

        void enter();

        void leave();

        bool allowMinorSyntaxErrors() const;

        JsonReader& setAllowMinorSyntaxErrors(bool value);

        bool allowComments() const;

        JsonReader& setAllowComments(bool value);

        bool allowExtendedValues() const;

        JsonReader& setAllowExtendedValues(bool value);

        size_t lineNumber() const;

        size_t columnNumber() const;

        void read(bool& value) const;

        void read(int8_t& value) const;

        void read(int16_t& value) const;

        void read(int32_t& value) const;

        void read(int64_t& value) const;

        void read(uint8_t& value) const;

        void read(uint16_t& value) const;

        void read(uint32_t& value) const;

        void read(uint64_t& value) const;

        void read(float& value) const;

        void read(double& value) const;

        void read(std::string& value) const;

        template<typename T>
        void readArray(std::vector<T>& values)
        {
            enter();
            while (nextValue())
            {
                T value;
                read(value);
                values.push_back(std::move(value));
            }
            leave();
        }

        bool nextDocument();

    protected:
        bool fillBuffer();

    private:
        void processStartArray();

        void processEndArray();

        void processStartObject();

        void processEndObject();

        void processString();

        void processValue();

        void processColon();

        void processComma();

        void processWhitespace();

        void skipElement();

        template <typename T>
        void readUnsignedInteger(T& value) const;

        template<typename T>
        void readSignedInteger(T& value) const;

        enum State
        {
            INITIAL_STATE,
            START_OF_DOCUMENT,
            AFTER_OBJECT_START,
            AFTER_OBJECT_ENTRY,
            BEFORE_OBJECT_KEY,
            AFTER_OBJECT_KEY,
            BEFORE_OBJECT_COLON,
            AFTER_OBJECT_COLON,
            AFTER_OBJECT_VALUE,
            BEFORE_OBJECT_COMMA,
            AFTER_OBJECT_COMMA,
            AFTER_ARRAY_START,
            AFTER_ARRAY_ENTRY,
            BEFORE_ARRAY_VALUE,
            AFTER_ARRAY_VALUE,
            BEFORE_ARRAY_COMMA,
            AFTER_ARRAY_COMMA,
            END_OF_DOCUMENT
        };

        std::unique_ptr<TextReader> m_TextReader;
        std::string m_Buffer;
        JsonTokenizer m_Tokenizer;
        State m_State = INITIAL_STATE;
        std::stack<State> m_StateStack;
        std::stack<ValueType_t> m_EnteredElements;
        size_t m_LineNumber = 1;
        size_t m_ColumnNumber = 1;
        bool m_AllowMinorSyntaxErrors = false;
        bool m_AllowComments = false;
        bool m_AllowExtendedValues = false;
    };
}
