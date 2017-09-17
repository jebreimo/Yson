//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "../Common/ThrowYsonException.hpp"
#include "../YsonReaderException.hpp"
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    #define UBJSON_READER_THROW(msg, tokenizer) \
        throw YsonReaderException((msg), \
                                  __FILE__, __LINE__, __FUNCTION__, \
                                  (tokenizer).fileName(), \
                                  0, \
                                  (tokenizer).position())

    #define UBJSON_READER_UNEXPECTED_TOKEN(tokenizer) \
        throw YsonReaderException( \
                "Unexpected token: " + toString((tokenizer).tokenType()) + ".", \
                __FILE__, __LINE__, __FUNCTION__, \
                (tokenizer).fileName(), \
                0, \
                (tokenizer).position())

    #define UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
        throw YsonReaderException("Unexpected end of document.",\
                                  __FILE__, __LINE__, __FUNCTION__, \
                                  (tokenizer).fileName(), \
                                  0, \
                                  (tokenizer).position())
}
