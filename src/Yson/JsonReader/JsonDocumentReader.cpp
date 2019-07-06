//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <vector>
#include "JsonDocumentReader.hpp"
#include "ThrowJsonReaderException.hpp"
#include "JsonScopeReaderUtilities.hpp"

namespace Yson
{
    std::pair<JsonReaderState, bool> JsonDocumentReader::nextKey(
            JsonTokenizer& tokenizer,
            JsonReaderState /*state*/)
    {
        JSON_READER_THROW("This function can only be called inside objects.",
                          tokenizer);
    }

    std::pair<JsonReaderState, bool> JsonDocumentReader::nextValue(
            JsonTokenizer& tokenizer,
            JsonReaderState state)
    {
        switch (state)
        {
        case JsonReaderState::INITIAL_STATE:
            if (!tokenizer.next())
                return {JsonReaderState::AT_END_OF_FILE, false};
            //[[fallthrough]]
        case JsonReaderState::AT_START:
            if (isValueToken(tokenizer.tokenType()))
                return {JsonReaderState::AT_VALUE, true};
            JSON_READER_THROW(
                    "Unexpected token '" + tokenizer.tokenString() + "'.",
                    tokenizer);
        case JsonReaderState::AT_END:
        case JsonReaderState::AT_END_OF_FILE:
        case JsonReaderState::AFTER_VALUE:
            return {state, false};
        case JsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            if (!tokenizer.next())
                return {JsonReaderState::AT_END_OF_FILE, false};
            return {JsonReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        };
    }

    std::pair<JsonReaderState, bool> JsonDocumentReader::nextDocument(
            JsonTokenizer& tokenizer,
            JsonReaderState state)
    {
        switch (state)
        {
        case JsonReaderState::INITIAL_STATE:
            if (!tokenizer.next())
                return {JsonReaderState::AT_END_OF_FILE, false};
            return {JsonReaderState::AT_START, true};
        case JsonReaderState::AT_END_OF_FILE:
            return {JsonReaderState::AT_END_OF_FILE, false};
        case JsonReaderState::AT_VALUE:
        case JsonReaderState::AT_START:
            skipValue(tokenizer);
            // [[fallthrough]]
        case JsonReaderState::AFTER_VALUE:
            if (!tokenizer.next())
                return {JsonReaderState::AT_END_OF_FILE, false};
            // [[fallthrough]]
        case JsonReaderState::AT_END:
            return {JsonReaderState::AT_START, true};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        };
    }
}
