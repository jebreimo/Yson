//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 01.11.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "BinaryMutableBufferReader.hpp"
#include <cassert>
#include "FromBigEndian.hpp"

namespace Yson
{
    BinaryMutableBufferReader::BinaryMutableBufferReader(char* buffer, size_t size)
            : m_TokenStart(buffer),
              m_TokenEnd(buffer),
              m_BufferStart(buffer),
              m_BufferEnd(buffer + size)
    {}

    bool BinaryMutableBufferReader::advance(size_t size)
    {
        auto actualSize = std::min<size_t>(size, m_BufferEnd - m_TokenEnd);
        m_TokenStart = m_TokenEnd += actualSize;
        return actualSize == size;
    }

    const void* BinaryMutableBufferReader::data() const
    {
        return m_TokenStart;
    }

    char BinaryMutableBufferReader::front() const
    {
        assert(m_TokenStart != m_TokenEnd);
        return *m_TokenStart;
    }

    bool BinaryMutableBufferReader::peek(char* value)
    {
        if (m_TokenEnd != m_BufferEnd)
        {
            *value = *m_TokenEnd;
            return true;
        }
        return false;
    }

    size_t BinaryMutableBufferReader::position() const
    {
        return m_TokenStart - m_BufferStart;
    }

    bool BinaryMutableBufferReader::read(size_t size, size_t unitSize)
    {
        auto actualSize = std::min<size_t>(size, m_BufferEnd - m_TokenEnd);
        m_TokenStart = m_TokenEnd;
        m_TokenEnd += actualSize;
        if (unitSize > 1)
            fromBigEndian(actualSize, m_TokenStart, unitSize);
        return actualSize == size;
    }

    bool BinaryMutableBufferReader::read(void* buffer, size_t size,
                                         size_t unitSize)
    {
        auto actualSize = std::min<size_t>(size, m_BufferEnd - m_TokenEnd);
        m_TokenStart = m_TokenEnd;
        m_TokenEnd += actualSize;
        if (actualSize != size)
        {
            // Moves m_TokenStart to the end of the buffer to "emulate"
            // BinaryStreamReader's behaviour.
            m_TokenStart = m_TokenEnd;
            return false;
        }
        memcpy(buffer, m_TokenStart, size);
        if (unitSize != 1)
            fromBigEndian(size, static_cast<char*>(buffer), unitSize);
        m_TokenStart = m_BufferEnd;
        return true;
    }

    size_t BinaryMutableBufferReader::size()
    {
        return m_TokenEnd - m_TokenStart;
    }
}
