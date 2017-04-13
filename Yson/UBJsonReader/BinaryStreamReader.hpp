//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.11.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <vector>
#include "BinaryReader.hpp"

namespace Yson
{
    class BinaryStreamReader : public BinaryReader
    {
    public:
        BinaryStreamReader(std::istream& stream);

        bool advance(size_t count) override;

        bool peek(char* value) override;

        bool read(size_t size, size_t unitSize) override;

        char front() const override;

        const void* data() const override;

        size_t position() const override;

        size_t size() override;

        bool read(void* buffer, size_t size, size_t unitSize) override;

    protected:
        BinaryStreamReader();

        void setStream(std::istream* stream);

    private:
        bool fillBuffer(size_t size);

        size_t remainingBytes() const;

        std::istream* m_Stream;
        std::vector<char> m_Buffer;
        char* m_Start;
        char* m_End;
    };
}
