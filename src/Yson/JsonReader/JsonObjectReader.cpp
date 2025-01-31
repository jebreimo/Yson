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
    std::pair<ReaderState, bool>
    JsonObjectReader::nextKey(JsonTokenizer& tokenizer,
                              ReaderState state)
    {
        switch (state)
        {
        case ReaderState::AT_START:
            if (readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_KEY, true};
            return {ReaderState::AT_END, false};
        case ReaderState::AT_KEY:
            readColon(tokenizer);
            if (!tokenizer.next())
                JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            [[fallthrough]];
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_END, false};
            if (readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_KEY, true};
            return {ReaderState::AT_END, false};
        case ReaderState::AT_END:
            return {ReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    std::pair<ReaderState, bool>
    JsonObjectReader::nextValue(JsonTokenizer& tokenizer,
                                ReaderState state)
    {
        switch (state)
        {
        case ReaderState::AT_START:
            if (!readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_END, false};
            readColon(tokenizer);
            readStartOfValue(tokenizer);
            return {ReaderState::AT_VALUE, true};
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AFTER_VALUE:
            if (!readComma(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_END, false};
            if (!readKey(tokenizer, JsonTokenType::END_OBJECT))
                return {ReaderState::AT_END, false};
            [[fallthrough]];
        case ReaderState::AT_KEY:
            readColon(tokenizer);
            readStartOfValue(tokenizer);
            return {ReaderState::AT_VALUE, true};
        case ReaderState::AT_END:
            return {ReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    std::pair<ReaderState, bool>
    JsonObjectReader::nextDocument(JsonTokenizer& tokenizer,
                                   ReaderState state)
    {
        JSON_READER_THROW(
                "Cannot call this function inside arrays or objects.",
                tokenizer);
    }

    char JsonObjectReader::scopeType() const
    {
        return '{';
    }
}
