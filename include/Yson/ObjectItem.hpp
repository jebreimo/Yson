//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <deque>
#include <unordered_map>
#include "JsonItem.hpp"

namespace Yson
{
    class YSON_API ObjectItem
    {
    public:
        ObjectItem(std::deque<std::string> keys,
                   std::unordered_map<std::string_view, JsonItem> values);

        [[nodiscard]]
        const std::deque<std::string>& keys() const;

        [[nodiscard]]
        const std::unordered_map<std::string_view, JsonItem>& values() const;

    private:
        std::deque<std::string> m_Keys;
        std::unordered_map<std::string_view, JsonItem> m_Values;
    };
}
