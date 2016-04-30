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
#include <memory>
#include <stack>
#include <string>
#include "../Ystring/Encoding.hpp"
#include "JsonReaderException.hpp"
#include "JsonTokenizer.hpp"
#include "LineNumberCounter.hpp"
#include "ValueType.hpp"

/** @file
  * @brief Defines the JsonReader class.
  */

namespace Yson {

    class TextReader;

    /** @brief A class for iterating efficiently over the contents of a JSON
      *   file.
      */
    class YSON_API JsonReader
    {
    public:
        JsonReader(std::istream& stream);

        JsonReader(const std::string& fileName);

        ~JsonReader();

        /** @brief Advances the reader to the next key in the most recently
          *     entered JSON object.
          * @throws JsonReaderException if it encounters an error in the JSON
          *     file or the most recently entered object isn't an object.
          */
        bool nextKey();

        /** @brief Advances the reader to the next value in the most recently
          *     entered JSON array or JSON object.
          */
        bool nextValue();

        bool nextToken();

        void enter();

        void leave();

        ValueType_t valueType() const;

        JsonTokenType_t tokenType() const;

        std::string token() const;

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

        bool nextDocument();

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

        bool isBlockStringsEnabled() const;

        JsonReader& setBlockStringsEnabled(bool value);

        int languageExtensions() const;

        JsonReader& setLanguageExtensions(int value);

    private:
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

        bool fillBuffer();

        bool nextTokenImpl();

        void processBlockString();

        void processColon();

        void processComma();

        void processEndArray();

        void processEndObject();

        void processEndOfStream();

        void processStartArray();

        void processStartObject();

        void processString();

        void processValue();

        void processWhitespace();

        void skipElement();

        void skipWhitespace();

        std::pair<const char*, const char*> getValueToken() const;

        template <typename T>
        void readUnsignedInteger(T& value) const;

        template<typename T>
        void readSignedInteger(T& value) const;

        enum State
        {
            INITIAL_STATE,
            AT_END_OF_STREAM,
            UNRECOVERABLE_ERROR,

            AT_START_OF_DOCUMENT,
            AT_VALUE_OF_DOCUMENT,
            AT_END_OF_DOCUMENT,

            AT_START_OF_OBJECT,
            AT_KEY_IN_OBJECT,
            AFTER_KEY_IN_OBJECT,
            AT_COLON_IN_OBJECT,
            AT_VALUE_IN_OBJECT,
            AFTER_VALUE_IN_OBJECT,
            AT_COMMA_IN_OBJECT,
            AT_END_OF_OBJECT,

            AT_START_OF_ARRAY,
            AT_VALUE_IN_ARRAY,
            AFTER_VALUE_IN_ARRAY,
            AT_COMMA_IN_ARRAY,
            AT_END_OF_ARRAY,

            AT_END_OF_NULL
        };

        std::unique_ptr<TextReader> m_TextReader;
        std::string m_Buffer;
        JsonTokenizer m_Tokenizer;
        State m_State;
        std::stack<State> m_StateStack;
        std::stack<ValueType_t> m_EnteredElements;
        LineNumberCounter m_LineNumberCounter;
        int m_LanguagExtentions;
        int m_SkipElementDepth;
    };

    template <typename T>
    T read(JsonReader& reader)
    {
        T value;
        reader.read(value);
        return value;
    }
}
