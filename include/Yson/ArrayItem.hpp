//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonItem.hpp"

namespace Yson
{
    /**
     * ArrayItem provides read access to an array in a JSON or UBJSON
     * document. It is returned by JsonItem::array() if the JsonItem is an
     * array.
     */
    class YSON_API ArrayItem
    {
    public:
        using iterator = std::vector<JsonItem>::const_iterator;

        explicit ArrayItem(std::vector<JsonItem> values);

        [[nodiscard]]
        const std::vector<JsonItem>& values() const;

        [[nodiscard]] size_t empty() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;
    private:
        std::vector<JsonItem> m_Values;
    };
}
