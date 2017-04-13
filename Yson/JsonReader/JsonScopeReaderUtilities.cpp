//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonScopeReaderUtilities.hpp"

#include <vector>
#include "JsonReaderException.hpp"

namespace Yson
{
    bool isValueToken(JsonTokenType tokenType)
    {
        switch (tokenType)
        {
        case JsonTokenType::START_ARRAY:
        case JsonTokenType::START_OBJECT:
        case JsonTokenType::STRING:
        case JsonTokenType::VALUE:
            return true;
        default:
            return false;
        }
    }

    void readColon(JsonTokenizer& tokenizer)
    {
        if (!tokenizer.next())
            JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        if (tokenizer.tokenType() == JsonTokenType::COLON)
            return;
        JSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    bool readComma(JsonTokenizer& tokenizer, JsonTokenType endToken)
    {
        if (!tokenizer.next())
            JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        if (tokenizer.tokenType() == JsonTokenType::COMMA)
            return true;
        if (tokenizer.tokenType() == endToken)
            return false;
        JSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    bool readKey(JsonTokenizer& tokenizer, JsonTokenType endToken)
    {
        if (!tokenizer.next())
            JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        if (tokenizer.tokenType() == JsonTokenType::STRING
            || tokenizer.tokenType() == JsonTokenType::VALUE)
        {
            return true;
        }
        if (tokenizer.tokenType() == endToken)
            return false;
        JSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    bool readStartOfValue(JsonTokenizer& tokenizer, JsonTokenType endToken)
    {
        if (!tokenizer.next())
            JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        if (isValueToken(tokenizer.tokenType()))
            return true;
        if (tokenizer.tokenType() == endToken)
            return false;
        JSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    void skipValue(JsonTokenizer& tokenizer)
    {
        std::vector<JsonTokenType> tokens;
        auto type = tokenizer.tokenType();
        if (type == JsonTokenType::START_OBJECT)
            tokens.push_back(JsonTokenType::END_OBJECT);
        else if (type == JsonTokenType::START_ARRAY)
            tokens.push_back(JsonTokenType::END_ARRAY);
        else if (type == JsonTokenType::STRING
                 || type == JsonTokenType::VALUE)
            return;
        else
            JSON_READER_UNEXPECTED_TOKEN(tokenizer);

        while (tokenizer.next())
        {
            switch (tokenizer.tokenType())
            {
            case JsonTokenType::START_ARRAY:
                tokens.push_back(JsonTokenType::END_ARRAY);
                break;
            case JsonTokenType::START_OBJECT:
                tokens.push_back(JsonTokenType::END_OBJECT);
                break;
            case JsonTokenType::END_ARRAY:
            case JsonTokenType::END_OBJECT:
                if (tokens.back() != tokenizer.tokenType())
                    JSON_READER_UNEXPECTED_TOKEN(tokenizer);
                tokens.pop_back();
                if (tokens.empty())
                    return;
                break;
            default:
                break;
            }
        }
        JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
    }
}
