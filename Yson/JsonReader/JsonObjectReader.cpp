//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonObjectReader.hpp"
#include "ThrowJsonReaderException.hpp"
#include "JsonScopeReaderUtilities.hpp"

namespace Yson
{
    std::pair<JsonReaderState, bool>
    JsonObjectReader::nextKey(JsonTokenizer& tokenizer,
                              JsonReaderState state)
    {
        switch (state)
        {
        case JsonReaderState::AT_START:
            if (readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {JsonReaderState::AT_KEY, true};
            else
                return {JsonReaderState::AT_END, false};
        case JsonReaderState::AT_KEY:
            readColon(tokenizer);
            if (!tokenizer.next())
                JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            //[[fallthrough]]
        case JsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case JsonReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_OBJECT))
                return {JsonReaderState::AT_END, false};
            readKey(tokenizer);
            return {JsonReaderState::AT_KEY, true};
        case JsonReaderState::AT_END:
            return {JsonReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    std::pair<JsonReaderState, bool>
    JsonObjectReader::nextValue(JsonTokenizer& tokenizer,
                                JsonReaderState state)
    {
        switch (state)
        {
        case JsonReaderState::AT_START:
            if (!readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {JsonReaderState::AT_END, false};
            readColon(tokenizer);
            readStartOfValue(tokenizer);
            return {JsonReaderState::AT_VALUE, true};
        case JsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case JsonReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_OBJECT))
                return {JsonReaderState::AT_END, false};
            readKey(tokenizer);
            //[[fallthrough]]
        case JsonReaderState::AT_KEY:
            readColon(tokenizer);
            readStartOfValue(tokenizer);
            return {JsonReaderState::AT_VALUE, true};
        case JsonReaderState::AT_END:
            return {JsonReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    std::pair<JsonReaderState, bool>
    JsonObjectReader::nextDocument(JsonTokenizer& tokenizer,
                                   JsonReaderState state)
    {
        JSON_READER_THROW(
                "Cannot call this function inside arrays or objects.",
                tokenizer);
    }
}
