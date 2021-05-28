//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonArray.hpp"

namespace Yson
{
    JsonArray::JsonArray(std::vector<JsonItem> values)
        : m_Values(move(values))
    {}

    const std::vector<JsonItem>& JsonArray::values() const
    {
        return m_Values;
    }
}
