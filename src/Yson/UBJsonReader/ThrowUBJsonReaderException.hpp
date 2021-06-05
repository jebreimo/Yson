//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Yson/YsonReaderException.hpp"
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    #define UBJSON_READER_THROW(msg, tokenizer) \
        throw ::Yson::YsonReaderException((msg), \
                                          YSON_DEBUG_LOCATION(), \
                                          (tokenizer).fileName(), \
                                          0, \
                                          (tokenizer).position())

    #define UBJSON_READER_UNEXPECTED_TOKEN(tokenizer) \
        UBJSON_READER_THROW( \
            "Unexpected token: " + toString((tokenizer).tokenType()) + ".", \
            (tokenizer))

    #define UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
        UBJSON_READER_THROW("Unexpected end of document.", \
                            (tokenizer))
}
