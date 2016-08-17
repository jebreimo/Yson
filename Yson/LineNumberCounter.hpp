//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "YsonDefinitions.hpp"

namespace Yson
{
    class YSON_API LineNumberCounter
    {
    public:
        LineNumberCounter();

        LineNumberCounter(unsigned line, unsigned column);

        unsigned line() const;

        unsigned column() const;

        void applyNextLineAndColumn();

        void setLineAndColumn(unsigned line, unsigned column);

        void setNextLineAndColumn(unsigned line, unsigned column);

        void setNextLineAndColumnOffsets(unsigned lineOffset,
                                         unsigned columnOffset);

    private:
        unsigned m_Line;
        unsigned m_Column;
        unsigned m_NextLine;
        unsigned m_NextColumn;
    };

}
