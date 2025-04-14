//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.04.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Yson
{
    template <bool B, typename TrueType, typename FalseType>
    struct SelectTypeIf;

    template <typename TrueType, typename FalseType>
    struct SelectTypeIf<true, TrueType, FalseType>
    {
      using type = TrueType;
    };

    template <typename TrueType, typename FalseType>
    struct SelectTypeIf<false, TrueType, FalseType>
    {
      using type = FalseType;
    };
}
