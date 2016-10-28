//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonObject.hpp"

#include "JsonInvalidValue.hpp"
#include "YsonException.hpp"

namespace Yson
{
    namespace
    {
        const std::string ObjectTypeName = "Object";

        template <typename RndIt, typename T, typename UnaryFunc>
        RndIt lowerBound(RndIt beg, RndIt end, const T& value,
                         UnaryFunc keyFunc)
        {
            while (beg != end)
            {
                RndIt mid = beg + std::distance(beg, end) / 2;
                if (keyFunc(*mid) < value)
                    beg = mid + 1;
                else
                    end = mid;
            }
            return beg;
        }
    }

    JsonObject::JsonObject(std::vector<NamedValue>&& values)
        : m_Values(std::move(values))
    {}

    ValueType JsonObject::valueType() const
    {
        return ValueType::OBJECT;
    }

    const std::string& JsonObject::typeName() const
    {
        return ObjectTypeName;
    }

    bool JsonObject::empty() const
    {
        return m_Values.empty();
    }

    size_t JsonObject::size() const
    {
        return m_Values.size();
    }

    const JsonValue& JsonObject::operator[](const std::string& key) const
    {
        auto it = lowerBound(m_Values.begin(), m_Values.end(), key,
                             [](auto& p){return p.first;});
        if (it != m_Values.end() && it->first == key)
            return *it->second;
        return JsonInvalidValue::instance();
    }

    JsonObject::Iterator JsonObject::begin() const
    {
        return m_Values.begin();
    }

    JsonObject::Iterator JsonObject::end() const
    {
        return m_Values.end();
    }

    size_t JsonObject::updateStringRef(size_t offset)
    {
        for (auto& value : m_Values)
            offset = value.second->updateStringRef(offset);
        stable_sort(m_Values.begin(), m_Values.end(),
                    [](auto& a, auto& b) {return a.first < b.first;});
        return offset;
    }
}
