//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-12.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

namespace Yconvert
{
    std::pair<size_t, size_t>
    convert_string(const void *source, size_t source_size,
                   void *destination, size_t destination_size,
                   Converter &converter)
    {
        return converter.convert(source, source_size,
                                 destination, destination_size);
    }

    std::pair<size_t, size_t>
    convert_string(const void* source, size_t source_size,
                   Encoding source_encoding,
                   void* destination, size_t destination_size,
                   Encoding destination_encoding,
                   ErrorPolicy error_policy)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        return converter.convert(source, source_size,
                                 destination, destination_size);
    }
}
