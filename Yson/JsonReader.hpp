//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-09-25.
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
#include "JsonReaderException.hpp"
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

        void enter();

        void leave();

        ValueType_t valueType() const;

        JsonTokenType_t tokenType() const;

        bool isStringsAsValuesEnabled() const;

        JsonReader& setStringsAsValuesEnabled(bool value);

        bool isValuesAsStringsEnabled() const;

        JsonReader& setValuesAsStringsEnabled(bool value);

        bool isEndElementAfterCommaEnabled() const;

        JsonReader& setEndElementAfterCommaEnabled(bool value);

        bool isCommentsEnabled() const;

        JsonReader& setCommentsEnabled(bool value);

        bool isEnterNullEnabled() const;

        JsonReader& setEnterNullEnabled(bool value);

        bool isValuesAsKeysEnabled() const;

        JsonReader& setValuesAsKeysEnabled(bool value);

        bool isExtendedIntegersEnabled() const;

        JsonReader& setExtendedIntegersEnabled(bool value);

        int languageExtensions() const;

        JsonReader& setLanguageExtensions(int value);

        size_t lineNumber() const;

        size_t columnNumber() const;

        bool isNull() const;

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

    private:
        bool fillBuffer();

        void processEndOfBuffer();

        void processStartArray();

        void processEndArray();

        void processStartObject();

        void processEndObject();

        void processString();

        void processValue();

        void processColon();

        void processComma();

        void processWhitespace();

        bool nextTokenImpl();

        void skipElement();

        template <typename T>
        void readUnsignedInteger(T& value) const;

        template<typename T>
        void readSignedInteger(T& value) const;

        std::pair<const char*, const char*> getValueToken() const;

        enum State
        {
            INITIAL_STATE,
            AT_START_OF_DOCUMENT,
            AT_VALUE_OF_DOCUMENT,
            AT_START_OF_OBJECT,
            AT_KEY_IN_OBJECT,
            AFTER_KEY_IN_OBJECT,
            AT_COLON_IN_OBJECT,
            AT_VALUE_IN_OBJECT,
            AFTER_VALUE_IN_OBJECT,
            AT_COMMA_IN_OBJECT,
            AT_START_OF_ARRAY,
            AT_VALUE_IN_ARRAY,
            AFTER_VALUE_IN_ARRAY,
            AT_COMMA_IN_ARRAY,
            AT_END_OF_DOCUMENT,
            AT_END_OF_OBJECT,
            AT_END_OF_ARRAY,
            AT_END_OF_NULL,
            AT_END_OF_BUFFER,
            UNRECOVERABLE_ERROR
        };

        enum LanguageExtensions
        {
            STRINGS_AS_VALUES = 1,
            VALUES_AS_STRINGS = 2,
            END_ELEMENT_AFTER_COMMA = 4,
            COMMENTS = 8,
            BLOCK_STRINGS = 16,
            ENTER_NULL = 32,
            VALUES_AS_KEYS = 64,
            EXTENDED_INTEGERS = 128
        };

        bool languageExtension(LanguageExtensions ext) const;
        JsonReader& setLanguageExtension(LanguageExtensions ext, bool value);

        std::unique_ptr<TextReader> m_TextReader;
        std::string m_Buffer;
        JsonTokenizer m_Tokenizer;
        State m_State = INITIAL_STATE;
        std::stack<State> m_StateStack;
        std::stack<ValueType_t> m_EnteredElements;
        size_t m_LineNumber = 1;
        size_t m_ColumnNumber = 1;
        int m_LanguagExtentions = 0;
        int m_SkipElementDepth = 0;
    };

    template <typename T>
    T read(JsonReader& reader)
    {
        T value;
        reader.read(value);
        return value;
    }
}
