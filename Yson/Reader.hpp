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
#include "ReaderException.hpp"
#include "Tokenizer.hpp"
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
    class YSON_API Reader
    {
    public:
        Reader(std::istream& stream);

        Reader(const std::string& fileName);

        ~Reader();

        /** @brief Advances the reader to the next key in the most recently
          *     entered JSON object.
          * @throws JsonReaderException if it encounters an error in the JSON
          *     file or the most recently entered object isn't an object.
          * @return false if the reader has reached the end of the most
          *     recently entered object.
          *
          *
          */
        bool nextKey();

        /** @brief Advances the reader to the next value in the most recently
          *     entered JSON array or object.
          * @return false if the reader has reached the end of the array or
          *     object.
          */
        bool nextValue();

        /** @brief Advances the reader to the next token in the most recently
          *     entered JSON array or object.
          * @return false if the reader has reached the end of the array or
          *     object.
          *
          * This function combined with the tokenType and token functions
          * provides low-level access to JSON contents such as commas,
          * colons, whitespace and comments.
          */
        bool nextToken();

        /** @brief Enters the current JSON array or object.
          *
          */
        void enter();

        /** @brief Leaves the current array or object.
          *
          */
        void leave();

        /** @brief Returns the type of the current value.
          *
          * @return The type of the current value.
          */
        ValueType valueType() const;

        /** @brief Tries to determine the Returns the type of the current value.
          *
          * @return The type of the current value.
          */
        ValueType stringValueType() const;

        TokenType tokenType() const;

        std::string token() const;

        size_t lineNumber() const;

        size_t columnNumber() const;

        /** @brief Returns true if the current value is null.
          * @return
          */
        bool isNull() const;

        void readValue(bool& value) const;

        void readValue(int8_t& value) const;

        void readValue(int16_t& value) const;

        void readValue(int32_t& value) const;

        void readValue(int64_t& value) const;

        void readValue(uint8_t& value) const;

        void readValue(uint16_t& value) const;

        void readValue(uint32_t& value) const;

        void readValue(uint64_t& value) const;

        void readValue(float& value) const;

        void readValue(double& value) const;

        void readValue(long double& value) const;

        void readValue(std::string& value) const;

        void readBase64(std::vector<uint8_t>& value) const;

        bool nextDocument();

        bool isStringsAsValuesEnabled() const;

        Reader& setStringsAsValuesEnabled(bool value);

        bool isValuesAsStringsEnabled() const;

        Reader& setValuesAsStringsEnabled(bool value);

        bool isEndElementAfterCommaEnabled() const;

        Reader& setEndElementAfterCommaEnabled(bool value);

        bool isCommentsEnabled() const;

        Reader& setCommentsEnabled(bool value);

        bool isEnterNullEnabled() const;

        Reader& setEnterNullEnabled(bool value);

        bool isValuesAsKeysEnabled() const;

        Reader& setValuesAsKeysEnabled(bool value);

        bool isExtendedIntegersEnabled() const;

        Reader& setExtendedIntegersEnabled(bool value);

        bool isBlockStringsEnabled() const;

        Reader& setBlockStringsEnabled(bool value);

        bool isExtendedFloatsEnabled() const;

        Reader& setExtendedFloatsEnabled(bool value);

        int languageExtensions() const;

        Reader& setLanguageExtensions(int value);

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
            EXTENDED_INTEGERS = 128,
            EXTENDED_FLOATS = 256
        };

        bool languageExtension(LanguageExtensions ext) const;

        Reader& setLanguageExtension(LanguageExtensions ext, bool value);

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

        std::tuple<const char*, const char*, bool> getValueToken() const;

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
        Tokenizer m_Tokenizer;
        std::stack<State> m_StateStack;
        LineNumberCounter m_LineNumberCounter;
        State m_State;
        int m_LanguageExtensions;
        int m_SkipElementDepth;
    };

    template <typename T>
    T read(Reader& reader)
    {
        T value;
        reader.readValue(value);
        return value;
    }
}
