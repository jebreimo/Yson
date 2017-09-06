//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>
#include "../YsonReaderException.hpp"

/** @file
  * @brief Defines macros and functions used when JsonReader
  *     throws exceptions.
  */

namespace Yson
{
    #define JSON_READER_THROW(msg, tokenizer) \
        throw YsonReaderException((msg), \
                                  __FILE__, __LINE__, __FUNCTION__, \
                                  (tokenizer).fileName(), \
                                  (tokenizer).lineNumber(), \
                                  (tokenizer).columnNumber())

    #define JSON_READER_UNEXPECTED_TOKEN(tokenizer) \
        throw YsonReaderException( \
                "Unexpected token: '" + (tokenizer).tokenString() + "'.", \
                __FILE__, __LINE__, __FUNCTION__, \
                (tokenizer).fileName(), \
                (tokenizer).lineNumber(), \
                (tokenizer).columnNumber())

    #define JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
        throw YsonReaderException("Unexpected end of document.",\
                                  __FILE__, __LINE__, __FUNCTION__, \
                                  (tokenizer).fileName(), \
                                  (tokenizer).lineNumber(), \
                                  (tokenizer).columnNumber())
}
