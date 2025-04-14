//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>
#include "Yson/YsonReaderException.hpp"

/** @file
  * @brief Defines macros and functions used when JsonReader
  *     throws exceptions.
  */

#define JSON_READER_THROW(msg, tokenizer) \
    throw ::Yson::YsonReaderException((msg), \
                                      YSON_DEBUG_LOCATION(), \
                                      (tokenizer).fileName(), \
                                      (tokenizer).lineNumber(), \
                                      (tokenizer).columnNumber())

#define JSON_READER_UNEXPECTED_TOKEN(tokenizer) \
    JSON_READER_THROW( \
            "Unexpected token: '" + (tokenizer).tokenString() + "'.", \
            (tokenizer))

#define JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
    JSON_READER_THROW("Unexpected end of document.", (tokenizer))
