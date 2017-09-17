//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 25.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonArrayReader.hpp"
#include "ThrowJsonReaderException.hpp"
#include "JsonScopeReaderUtilities.hpp"

namespace Yson
{
    std::pair<JsonReaderState, bool>
    JsonArrayReader::nextKey(JsonTokenizer& tokenizer, JsonReaderState state)
    {
        JSON_READER_THROW("This function only applies to entered objects.",
                          tokenizer);
    }

    std::pair<JsonReaderState, bool>
    JsonArrayReader::nextValue(JsonTokenizer& tokenizer,
                               JsonReaderState state)
    {
        switch (state)
        {
        case JsonReaderState::AT_START:
            if (!readStartOfValue(tokenizer, JsonTokenType::END_ARRAY))
                return {JsonReaderState::AT_END, false};
            return {JsonReaderState::AT_VALUE, true};
        case JsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case JsonReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_ARRAY))
                return {JsonReaderState::AT_END, false};
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
    JsonArrayReader::nextDocument(JsonTokenizer& tokenizer,
                                  JsonReaderState state)
    {
        JSON_READER_THROW(
                "Cannot call this function inside arrays or objects.",
                tokenizer);
    }
}
