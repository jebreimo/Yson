//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonObject.hpp"

namespace Yson
{
    JsonObject::JsonObject(std::deque<std::string> keys,
                           std::unordered_map<std::string_view, JsonItem> values)
        : m_Keys(move(keys)),
          m_Values(move(values))
    {}

    const std::deque<std::string>& JsonObject::keys() const
    {
        return m_Keys;
    }

    const std::unordered_map<std::string_view, JsonItem>&
    JsonObject::values() const
    {
        return m_Values;
    }
}
