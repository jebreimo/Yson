//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.11.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "BinaryStreamReader.hpp"
#include <cassert>
#include <istream>
#include "FromBigEndian.hpp"

namespace Yson
{
    namespace
    {
        constexpr size_t DEFAULT_BUFFER_SIZE = 64 * 1024;
    }

    BinaryStreamReader::BinaryStreamReader()
            : m_Stream(nullptr),
              m_Start(m_Buffer.data()),
              m_End(m_Buffer.data())
    {}

    BinaryStreamReader::BinaryStreamReader(std::istream& stream,
                                           const char* buffer,
                                           size_t bufferSize)
            : m_Stream(&stream),
              m_Buffer(buffer, buffer ? buffer + bufferSize : buffer),
              m_Start(m_Buffer.data()),
              m_End(m_Buffer.data())
    {}

    bool BinaryStreamReader::advance(size_t size)
    {
        m_Start = m_End;
        auto remainderSize = remainingBytes();
        if (size < remainderSize)
        {
            m_End = m_Start += size;
            return true;
        }
        m_End = m_Start += remainderSize;
        m_Stream->seekg(std::streamsize(size - remainderSize),
                        std::ios_base::cur);
        return m_Stream->gcount() == size - remainderSize;
    }

    const void* BinaryStreamReader::data() const
    {
        return m_Start;
    }

    char BinaryStreamReader::front() const
    {
        assert(m_Start != m_End);
        return *m_Start;
    }

    bool BinaryStreamReader::peek(char* value)
    {
        assert(value);
        if (remainingBytes() != 0 || fillBuffer(1))
        {
            *value = *m_End;
            return true;
        }
        return false;
    }

    size_t BinaryStreamReader::position() const
    {
        auto pos = m_Stream->tellg();
        if (pos != std::istream::pos_type(-1))
            return size_t(pos) - remainingBytes();
        return 0;
    }

    bool BinaryStreamReader::read(size_t size, size_t unitSize)
    {
        m_Start = m_End;
        auto remainderSize = remainingBytes();
        if (size > remainderSize)
        {
            if (!fillBuffer(size))
                return false;
        }
        m_End = m_Start + size;
        if (unitSize > 1)
            fromBigEndian(size, m_Start, unitSize);
        return true;
    }

    bool BinaryStreamReader::read(void* buffer, size_t size, size_t unitSize)
    {
        m_Start = m_End;
        auto remainderSize = remainingBytes();
        if (size <= remainderSize)
        {
            m_End = m_Start + size;
            memcpy(buffer, m_Start, size);
            if (unitSize > 1)
                fromBigEndian(size, m_Start, unitSize);
            return true;
        }

        if (remainderSize)
            memcpy(buffer, m_Start, size);
        m_End = m_Start += remainderSize;
        m_Stream->read(static_cast<char*>(buffer) + remainderSize,
                       size - remainderSize);
        auto readSize = size_t(m_Stream->gcount()) + remainderSize;
        if (readSize != size)
            return false;
        if (unitSize > 1)
            fromBigEndian(size, m_Start, unitSize);
        return true;
    }

    size_t BinaryStreamReader::size()
    {
        return m_End - m_Start;
    }

    void BinaryStreamReader::setStream(std::istream* stream)
    {
        m_Stream = stream;
    }

    bool BinaryStreamReader::fillBuffer(size_t size)
    {
        auto remainderSize = remainingBytes();
        if (m_Buffer.empty())
        {
            m_Buffer.resize(std::max(DEFAULT_BUFFER_SIZE, size));
        }
        else
        {
            if (remainderSize != 0 && m_Start != m_Buffer.data())
                std::copy(m_Start, m_Start + remainderSize, m_Buffer.data());
            if (m_Buffer.size() < size)
                m_Buffer.resize(size);
        }
        m_End = m_Buffer.data() + (m_End - m_Start);
        m_Start = m_Buffer.data();
        m_Stream->read(m_Buffer.data() + remainderSize,
                       m_Buffer.size() - remainderSize);
        auto contentSize = m_Stream->gcount() + remainderSize;
        if (contentSize < m_Buffer.size())
            m_Buffer.resize(contentSize);
        return contentSize >= size;
    }

    size_t BinaryStreamReader::remainingBytes() const
    {
        return size_t(m_Buffer.data() + m_Buffer.size() - m_Start);
    }
}
