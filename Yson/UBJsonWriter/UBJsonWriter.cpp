//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../UBJsonWriter.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include "../../Ystring/Conversion.hpp"
#include "../YsonException.hpp"
#include "../Common/Base64.hpp"
#include "../Common/GetUnicodeFileName.hpp"
#include "UBJsonWriterUtilities.hpp"

namespace Yson
{
    UBJsonWriter::UBJsonWriter()
        : m_Stream(&std::cout),
          m_StrictIntegerSizes(false)
    {}

    UBJsonWriter::UBJsonWriter(std::string& fileName)
        : m_StreamPtr(new std::ofstream(getUnicodeFileName(fileName),
                                        std::ios_base::binary)),
          m_Stream(m_StreamPtr.get()),
          m_StrictIntegerSizes(false)
    {}

    UBJsonWriter::UBJsonWriter(std::ostream& stream)
            : m_Stream(&stream),
              m_StrictIntegerSizes(false)
    {
        m_Contexts.push(Context());
    }

    UBJsonWriter::~UBJsonWriter()
    {
        flush();
    }

    std::ostream& UBJsonWriter::stream() const
    {
        flush();
        return *m_Stream;
    }

    const std::string& UBJsonWriter::key() const
    {
        return m_Key;
    }

    UBJsonWriter& UBJsonWriter::key(const std::string& key)
    {
        m_Key = key;
        return *this;
    }

    UBJsonWriter& UBJsonWriter::beginArray()
    {
        return beginStructure(UBJsonValueType::ARRAY, UBJsonParameters());
    }

    UBJsonWriter& UBJsonWriter::beginArray(const StructureParameters& params)
    {
        return beginStructure(UBJsonValueType::ARRAY,
                              params.ubJsonParameters);
    }

    UBJsonWriter& UBJsonWriter::endArray()
    {
        return endStructure(UBJsonValueType::ARRAY);
    }

    UBJsonWriter& UBJsonWriter::beginObject()
    {
        return beginStructure(UBJsonValueType::OBJECT, UBJsonParameters());
    }

    UBJsonWriter& UBJsonWriter::beginObject(
            const StructureParameters& parameters)
    {
        return beginStructure(UBJsonValueType::OBJECT,
                              parameters.ubJsonParameters);
    }

    UBJsonWriter& UBJsonWriter::endObject()
    {
        return endStructure(UBJsonValueType::OBJECT);
    }

    UBJsonWriter& UBJsonWriter::null()
    {
        beginValue();
        if (m_Contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m_Buffer.push_back('Z');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::boolean(bool value)
    {
        beginValue();
        if (m_Contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m_Buffer.push_back(value ? 'T' : 'F');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(char value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int8_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int16_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int32_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int64_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(uint8_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(uint16_t value)
    {
        if (value <= INT16_MAX)
            return writeInteger<int16_t>(value);
        else if (!m_StrictIntegerSizes)
            return writeInteger<int32_t>(value);
        YSON_THROW("uint16_t value " + std::to_string(value)
                   + " is greater than INT16_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(uint32_t value)
    {
        if (value <= INT32_MAX)
            return writeInteger<int32_t>(value);
        else if (!m_StrictIntegerSizes)
            return writeInteger<int64_t>(value);
        YSON_THROW("uint32_t value " + std::to_string(value)
                   + " is greater than INT32_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(uint64_t value)
    {
        if (value <= INT64_MAX)
            return writeInteger(static_cast<int64_t>(value));
        YSON_THROW("uint64_t value " + std::to_string(value)
                   + " is greater than INT64_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(float value)
    {
        return writeFloat(value);
    }

    UBJsonWriter& UBJsonWriter::value(double value)
    {
        return writeFloat(value);
    }

    UBJsonWriter& UBJsonWriter::value(const std::string& text)
    {
        beginValue();
        auto& context = m_Contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m_Buffer.push_back('S');
        writeMinimalInteger(m_Buffer, m_Key.size());
        m_Buffer.insert(m_Buffer.end(), m_Key.begin(), m_Key.end());
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(const std::wstring& text)
    {
        return value(Ystring::Conversion::convert<std::string>(
                text,
                Ystring::Encoding::UTF_16,
                Ystring::Encoding::UTF_8));
    }

    UBJsonWriter& UBJsonWriter::binary(const void* data, size_t size)
    {
        beginArray(UBJsonParameters(size, UBJsonValueType::UINT_8));
        flush();
        m_Stream->write(static_cast<const char*>(data), size);
        m_Contexts.top().index = size;
        return endArray();
    }

    UBJsonWriter& UBJsonWriter::base64(const void* data, size_t size)
    {
        return value(toBase64(data, size));
    }

    UBJsonWriter& UBJsonWriter::noop()
    {
        m_Buffer.push_back('N');
        flush();
        return *this;
    }

    bool UBJsonWriter::isStrictIntegerSizesEnabled() const
    {
        return m_StrictIntegerSizes;
    }

    UBJsonWriter& UBJsonWriter::setStrictIntegerSizesEnabled(bool value)
    {
        m_StrictIntegerSizes = value;
        return *this;
    }

    UBJsonWriter& UBJsonWriter::beginStructure(
            UBJsonValueType structureType,
            const UBJsonParameters& parameters)
    {
        assert(structureType == UBJsonValueType::OBJECT
               || structureType == UBJsonValueType::ARRAY);

        beginValue();
        auto& context = m_Contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m_Buffer.push_back(char(structureType));
        if (parameters.size >= 0)
        {
            if (parameters.valueType != UBJsonValueType::UNKNOWN)
            {
                m_Buffer.push_back('$');
                m_Buffer.push_back(char(parameters.valueType));
            }
            m_Buffer.push_back('#');
            writeMinimalInteger(m_Buffer, int64_t(parameters.size));
        }
        m_Contexts.push(Context(structureType,
                                parameters.size,
                                parameters.valueType));
        return *this;
    }

    UBJsonWriter& UBJsonWriter::endStructure(UBJsonValueType structureType)
    {
        auto endChar = structureType == UBJsonValueType::OBJECT ? '}' : ']';
        auto& context = m_Contexts.top();
        if (context.structureType == structureType)
        {
            if (context.size == -1)
            {
                m_Buffer.push_back(endChar);
            }
            else if (context.index != context.size)
            {
                if (context.index != 0
                    || (context.valueType != UBJsonValueType::NULL_VALUE
                        && context.valueType != UBJsonValueType::FALSE_VALUE
                        && context.valueType != UBJsonValueType::TRUE_VALUE))
                {
                    YSON_THROW("Too few items in optimized array. Has "
                               + std::to_string(context.index) + ", expects "
                               + std::to_string(context.size) + ".");
                }
            }
        }
        else
        {
            YSON_THROW("Ending structure " + toString(context.structureType)
                       + " as if it was a " + toString(structureType));
        }
        m_Contexts.pop();
        flush();
        return *this;
    }

    void UBJsonWriter::beginValue()
    {
        auto& context = m_Contexts.top();
        if (context.structureType == UBJsonValueType::OBJECT)
        {
            writeMinimalInteger(m_Buffer, m_Key.size());
            m_Buffer.insert(m_Buffer.end(), m_Key.begin(), m_Key.end());
        }
        m_Key.clear();
        if (context.index == context.size)
            YSON_THROW("Already at the end of optimized object or array.");
        ++context.index;
    }

    void UBJsonWriter::flush() const
    {
        if (m_Stream && !m_Buffer.empty())
        {
            m_Stream->write(m_Buffer.data(), m_Buffer.size());
            m_Buffer.clear();
        }
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeFloat(T value)
    {
        beginValue();
        auto& context = m_Contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            writeValueWithMarker(m_Buffer, value);
        else
            writeFloatAs(m_Buffer, value, context.valueType);
        return *this;
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeInteger(T value)
    {
        beginValue();
        auto& context = m_Contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
        {
            if (!m_StrictIntegerSizes)
                writeMinimalInteger(m_Buffer, value);
            else
                writeValueWithMarker(m_Buffer, value);
        }
        else
        {
            writeIntegerAs(m_Buffer, value, context.valueType);
        }
        return *this;
    }
}
