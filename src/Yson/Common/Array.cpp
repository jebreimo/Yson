//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/Array.hpp"

namespace Yson
{
    Array::Array(std::vector<JsonItem> values)
        : m_Values(move(values))
    {}

    const std::vector<JsonItem>& Array::values() const
    {
        return m_Values;
    }
}
