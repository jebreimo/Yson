//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 01.11.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "BinaryReader.hpp"

namespace Yson
{
    class BinaryMutableBufferReader : public BinaryReader
    {
    public:
        BinaryMutableBufferReader(char* buffer, size_t size);

        bool advance(size_t count) override;

        bool peek(char* value) override;

        bool read(size_t size, size_t unitSize) override;

        char front() const override;

        const void* data() const override;

        size_t position() const override;

        size_t size() override;

        bool read(void* buffer, size_t size, size_t unitSize) override;

    private:
        char* m_TokenStart;
        char* m_TokenEnd;
        char* m_BufferStart;
        char* m_BufferEnd;
    };
}
