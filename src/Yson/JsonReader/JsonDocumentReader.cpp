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
    std::pair<ReaderState, bool> JsonDocumentReader::nextKey(
            JsonTokenizer& tokenizer,
            ReaderState /*state*/)
    {
        JSON_READER_THROW("This function can only be called inside objects.",
                          tokenizer);
    }

    std::pair<ReaderState, bool> JsonDocumentReader::nextValue(
            JsonTokenizer& tokenizer,
            ReaderState state)
    {
        switch (state)
        {
        case ReaderState::INITIAL_STATE:
            if (!tokenizer.next())
                return {ReaderState::AT_END_OF_FILE, false};
            [[fallthrough]];
        case ReaderState::AT_START:
            if (isValueToken(tokenizer.tokenType()))
                return {ReaderState::AT_VALUE, true};
            JSON_READER_THROW(
                    "Unexpected token '" + tokenizer.tokenString() + "'.",
                    tokenizer);
        case ReaderState::AT_END:
        case ReaderState::AT_END_OF_FILE:
        case ReaderState::AFTER_VALUE:
            return {state, false};
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            if (!tokenizer.next())
                return {ReaderState::AT_END_OF_FILE, false};
            return {ReaderState::AT_END, false};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        };
    }

    std::pair<ReaderState, bool> JsonDocumentReader::nextDocument(
            JsonTokenizer& tokenizer,
            ReaderState state)
    {
        switch (state)
        {
        case ReaderState::INITIAL_STATE:
            if (!tokenizer.next())
                return {ReaderState::AT_END_OF_FILE, false};
            return {ReaderState::AT_START, true};
        case ReaderState::AT_END_OF_FILE:
            return {ReaderState::AT_END_OF_FILE, false};
        case ReaderState::AT_VALUE:
        case ReaderState::AT_START:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AFTER_VALUE:
            if (!tokenizer.next())
                return {ReaderState::AT_END_OF_FILE, false};
            [[fallthrough]];
        case ReaderState::AT_END:
            return {ReaderState::AT_START, true};
        default:
            JSON_READER_THROW("Invalid state: " + toString(state) + ".",
                              tokenizer);
        }
    }

    char JsonDocumentReader::scopeType() const
    {
        return '\0';
    }
}
