//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonItem.hpp"

namespace Yson
{
    class JsonArray
    {
    public:
        explicit JsonArray(std::vector<JsonItem> values);

        [[nodiscard]]
        const std::vector<JsonItem>& values() const;

    private:
        std::vector<JsonItem> m_Values;
    };
}
