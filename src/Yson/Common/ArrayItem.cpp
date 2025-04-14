//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/ArrayItem.hpp"

namespace Yson
{
    ArrayItem::ArrayItem(std::vector<JsonItem> values)
        : m_Values(std::move(values))
    {}

    const std::vector<JsonItem>& ArrayItem::values() const
    {
        return m_Values;
    }

    size_t ArrayItem::empty() const
    {
        return m_Values.empty();
    }

    size_t ArrayItem::size() const
    {
        return m_Values.size();
    }

    ArrayItem::iterator ArrayItem::begin() const
    {
        return m_Values.begin();
    }

    ArrayItem::iterator ArrayItem::end() const
    {
        return m_Values.end();
    }
}
