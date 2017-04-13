//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 13.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "../Common/AssignInteger.hpp"
#include "../Common/ParseFloatingPoint.hpp"
#include "../Common/ParseInteger.hpp"
#include "UBJsonReaderException.hpp"
#include "UBJsonReaderState.hpp"
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    template <typename T>
    bool setIntegerValue(T& value, const UBJsonTokenizer& tokenizer)
    {
        switch (tokenizer.tokenType())
        {
        case UBJsonTokenType::NULL_TOKEN:
            value = 0;
            return true;
        case UBJsonTokenType::TRUE_TOKEN:
            value = 1;
            return true;
        case UBJsonTokenType::FALSE_TOKEN:
            value = 0;
            return true;
        case UBJsonTokenType::CHAR_TOKEN:
        case UBJsonTokenType::INT8_TOKEN:
            return assignInteger<T, int8_t>(value, tokenizer.tokenAs<int8_t>());
        case UBJsonTokenType::UINT8_TOKEN:
            return assignInteger<T, uint8_t>(value, tokenizer.tokenAs<uint8_t>());
        case UBJsonTokenType::INT16_TOKEN:
            return assignInteger<T, int16_t>(value, tokenizer.tokenAs<int16_t>());
        case UBJsonTokenType::INT32_TOKEN:
            return assignInteger<T, int32_t>(value, tokenizer.tokenAs<int32_t>());
        case UBJsonTokenType::INT64_TOKEN:
            return assignInteger<T, int64_t>(value, tokenizer.tokenAs<int64_t>());
        case UBJsonTokenType::STRING_TOKEN:
            return parse(tokenizer.token(), value, true);
        default:
            UBJSON_READER_THROW("Not an integer value: "
                                + toString(tokenizer.tokenType()),
                                tokenizer);
        }
    }

    template <typename T>
    bool setFloatingPointValue(T& value, const UBJsonTokenizer& tokenizer)
    {
        switch (tokenizer.tokenType())
        {
        case UBJsonTokenType::NULL_TOKEN:
            value = std::numeric_limits<T>::infinity();
            return true;
        case UBJsonTokenType::TRUE_TOKEN:
            value = 1;
            return true;
        case UBJsonTokenType::FALSE_TOKEN:
            value = 0;
            return true;
        case UBJsonTokenType::CHAR_TOKEN:
        case UBJsonTokenType::INT8_TOKEN:
            value = T(tokenizer.tokenAs<int8_t>());
            return true;
        case UBJsonTokenType::UINT8_TOKEN:
            value = T(tokenizer.tokenAs<uint8_t>());
            return true;
        case UBJsonTokenType::INT16_TOKEN:
            value = T(tokenizer.tokenAs<int16_t>());
            return true;
        case UBJsonTokenType::INT32_TOKEN:
            value = T(tokenizer.tokenAs<int32_t>());
            return true;
        case UBJsonTokenType::INT64_TOKEN:
            value = T(tokenizer.tokenAs<int64_t>());
            return true;
        case UBJsonTokenType::FLOAT32_TOKEN:
            value = T(tokenizer.tokenAs<float>());
            return true;
        case UBJsonTokenType::FLOAT64_TOKEN:
            value = T(tokenizer.tokenAs<double>());
            return true;
        case UBJsonTokenType::STRING_TOKEN:
        case UBJsonTokenType::HIGH_PRECISION_TOKEN:
            return parse(tokenizer.token(), value);
        default:
            UBJSON_READER_THROW("Not a floating point value: "
                                + toString(tokenizer.tokenType()), tokenizer);
        }
    }

    UBJsonReaderState makeState(const UBJsonTokenizer& tokenizer);
}
