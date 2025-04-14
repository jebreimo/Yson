//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 25.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonArrayReader.hpp"
#include "ThrowJsonReaderException.hpp"
#include "JsonScopeReaderUtilities.hpp"

namespace Yson
{
    std::pair<ReaderState, bool>
    JsonArrayReader::nextKey(JsonTokenizer& tokenizer, ReaderState state)
    {
        JSON_READER_THROW("This function only applies to entered objects.",
                          tokenizer);
    }

    std::pair<ReaderState, bool>
    JsonArrayReader::nextValue(JsonTokenizer& tokenizer,
                               ReaderState state)
    {
        switch (state)
        {
        case ReaderState::AT_START:
            if (!readStartOfValue(tokenizer, JsonTokenType::END_ARRAY))
                return {ReaderState::AT_END, false};
            return {ReaderState::AT_VALUE, true};
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_ARRAY))
                return {ReaderState::AT_END, false};
            if (readStartOfValue(tokenizer, JsonTokenType::END_ARRAY))
                return {ReaderState::AT_VALUE, true};
            return {ReaderState::AT_END, false};
        case ReaderState::AT_END:
            return {ReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    std::pair<ReaderState, bool>
    JsonArrayReader::nextDocument(JsonTokenizer& tokenizer,
                                  ReaderState state)
    {
        JSON_READER_THROW(
                "Cannot call this function inside arrays or objects.",
                tokenizer);
    }

    char JsonArrayReader::scopeType() const
    {
        return '[';
    }
}
