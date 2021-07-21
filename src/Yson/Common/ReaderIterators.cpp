//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-06-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/ReaderIterators.hpp"

namespace Yson
{
    namespace detail
    {
        bool initialize(Reader& reader)
        {
            auto state = reader.state();
            if (state == ReaderState::INITIAL_STATE
                || (state == ReaderState::AT_START
                    && reader.scope().empty()))
            {
                if (!reader.nextValue())
                    return false;
            }
            reader.enter();
            return true;
        }
    }

    ObjectKeyIterator::ObjectKeyIterator()
        : m_Reader(nullptr),
          m_AtEnd(true)
    {}

    ObjectKeyIterator::ObjectKeyIterator(Reader & reader)
        : m_Reader(&reader),
          m_AtEnd(!detail::initialize(*m_Reader))
    {
        if (!m_AtEnd)
            operator++();
    }

    const std::string& ObjectKeyIterator::operator*() const
    {
        return m_Key;
    }

    const std::string* ObjectKeyIterator::operator->() const
    {
        return &m_Key;
    }

    ObjectKeyIterator& ObjectKeyIterator::operator++()
    {
        if (!m_AtEnd)
        {
            if (m_Reader->nextKey())
            {
                m_Reader->get(m_Key);
                m_Reader->nextValue();
            }
            else
            {
                m_Reader->leave();
                m_AtEnd = true;
            }
        }

        return *this;
    }

    bool operator==(const ObjectKeyIterator& a,
                    const ObjectKeyIterator& b)
    {
        return &a == &b || (a.m_AtEnd && b.m_AtEnd);
    }

    bool operator!=(const ObjectKeyIterator& a,
                    const ObjectKeyIterator& b)
    {
        return !(a == b);
    }

    ObjectKeyIteratorAdapter::ObjectKeyIteratorAdapter(Reader& reader)
        : m_Reader(&reader)
    {}

    ObjectKeyIterator ObjectKeyIteratorAdapter::begin()
    {
        if (!m_Reader)
            YSON_THROW("begin() can only be called once.");

        auto* reader = m_Reader;
        m_Reader = nullptr;
        return ObjectKeyIterator(*reader);
    }

    ObjectKeyIterator ObjectKeyIteratorAdapter::end() const
    {
        return ObjectKeyIterator();
    }

    ArrayIterator::ArrayIterator(Reader& reader)
        : m_Reader(&reader),
          m_State(0)
    {}

    bool ArrayIterator::next()
    {
        if (m_State == 0)
            m_State = detail::initialize(*m_Reader) ? 1 : 2;

        if (m_State == 1)
        {
            if (m_Reader->nextValue())
                return true;

            m_State = 2;
            m_Reader->leave();
        }

        return false;
    }
}
