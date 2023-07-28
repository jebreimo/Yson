//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/ObjectItem.hpp"

namespace Yson
{
    ObjectItem::ObjectItem(std::deque<std::string> keys,
                           std::unordered_map<std::string_view, JsonItem> values)
        : m_Keys(std::move(keys)),
          m_Values(std::move(values))
    {}

    const std::deque<std::string>& ObjectItem::keys() const
    {
        return m_Keys;
    }

    const std::unordered_map<std::string_view, JsonItem>&
    ObjectItem::values() const
    {
        return m_Values;
    }

    size_t ObjectItem::empty() const
    {
        return m_Values.empty();
    }

    size_t ObjectItem::size() const
    {
        return m_Values.size();
    }

    ObjectItem::iterator ObjectItem::begin() const
    {
        return m_Values.begin();
    }

    ObjectItem::iterator ObjectItem::end() const
    {
        return m_Values.end();
    }
}
