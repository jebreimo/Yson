//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-06-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Reader.hpp"

namespace Yson
{
    namespace detail
    {
        YSON_API bool initialize(Reader& reader);
    }

    class YSON_API ObjectKeyIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = void;
        using pointer = const std::string*;
        using reference = const std::string&;

        ObjectKeyIterator();

        explicit ObjectKeyIterator(Reader& reader);

        [[nodiscard]]
        const std::string& operator*() const;

        [[nodiscard]]
        const std::string* operator->() const;

        ObjectKeyIterator& operator++();

        friend bool operator==(const ObjectKeyIterator& a,
                               const ObjectKeyIterator& b);

        [[nodiscard]]
        ObjectKeyIterator& begin();

        [[nodiscard]]
        static ObjectKeyIterator end();
    private:
        Reader* m_Reader;
        std::string m_Key;
        bool m_AtEnd;
    };

    YSON_API bool operator!=(const ObjectKeyIterator& a,
                             const ObjectKeyIterator& b);

    inline ObjectKeyIterator keys(Reader& reader)
    {
        return ObjectKeyIterator(reader);
    }

    class YSON_API ArrayIterator
    {
    public:
        explicit ArrayIterator(Reader& reader);

        bool next();
    private:
        Reader* m_Reader;
        int m_State;
    };

    template <typename T>
    class ArrayValueIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = void;
        using pointer = const T*;
        using reference = const T&;

        ArrayValueIterator()
            : m_Reader(),
              m_Value(),
              m_AtEnd(true)
        {}

        explicit ArrayValueIterator(Reader& reader)
            : m_Reader(&reader),
              m_Value(),
              m_AtEnd(!detail::initialize(*m_Reader))
        {
            if (!m_AtEnd)
                operator++();
        }

        [[nodiscard]]
        const T& operator*() const
        {
            return m_Value;
        }

        [[nodiscard]]
        const T* operator->() const
        {
            return &m_Value;
        }

        ArrayValueIterator& operator++()
        {
            if (!m_AtEnd)
            {
                if (m_Reader->nextValue())
                {
                    m_Value = read<T>(*m_Reader);
                }
                else
                {
                    m_Reader->leave();
                    m_AtEnd = true;
                }
            }

            return *this;
        }

        friend bool operator==(const ArrayValueIterator& a,
                               const ArrayValueIterator& b)
        {
            return &a == &b || (a.m_AtEnd && b.m_AtEnd);
        }

        friend bool operator!=(const ArrayValueIterator& a,
                               const ArrayValueIterator& b)
        {
            return !(a == b);
        }

        ArrayValueIterator& begin()
        {
            return *this;
        }

        static ArrayValueIterator end()
        {
            return {};
        }
    private:
        Reader* m_Reader;
        T m_Value;
        bool m_AtEnd;
    };

    template <typename T>
    ArrayValueIterator<T> arrayValues(Reader& reader)
    {
        return ArrayValueIterator<T>(reader);
    }
}
