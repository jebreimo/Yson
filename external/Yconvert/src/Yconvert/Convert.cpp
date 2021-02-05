//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

namespace Yconvert
{
    std::pair<size_t, size_t>
    convertString(const void* source, size_t sourceSize,
                  Encoding sourceEncoding,
                  void* destination, size_t destinationSize,
                  Encoding destinationEncoding,
                  ErrorPolicy errorPolicy)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        return converter.convert(source, sourceSize,
                                 destination, destinationSize);
    }
}
