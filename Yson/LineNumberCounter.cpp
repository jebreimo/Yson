//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "LineNumberCounter.hpp"

namespace Yson {

    LineNumberCounter::LineNumberCounter()
        : m_Line(1),
          m_Column(1),
          m_NextLine(1),
          m_NextColumn(1)
    {}

    LineNumberCounter::LineNumberCounter(unsigned line, unsigned column)
        : m_Line(line),
          m_Column(column),
          m_NextLine(line),
          m_NextColumn(column)
    {}

    unsigned LineNumberCounter::line() const
    {
        return m_Line;
    }

    unsigned LineNumberCounter::column() const
    {
        return m_Column;
    }

    void LineNumberCounter::applyNextLineAndColumn()
    {
        m_Line = m_NextLine;
        m_Column = m_NextColumn;
    }

    void LineNumberCounter::setLineAndColumn(unsigned line, unsigned column)
    {
        m_Line = line;
        m_Column = column;
    }

    void LineNumberCounter::setNextLineAndColumn(unsigned line,
                                                 unsigned column)
    {
        m_NextLine = line;
        m_NextColumn = column;
    }

    void LineNumberCounter::setNextLineAndColumnOffsets(unsigned lineOffset,
                                                        unsigned columnOffset)
    {
        if (lineOffset == 0)
        {
            m_NextColumn = m_Column + columnOffset;
        }
        else
        {
            m_NextLine = m_Line + lineOffset;
            m_NextColumn = columnOffset + 1;
        }
    }
}
