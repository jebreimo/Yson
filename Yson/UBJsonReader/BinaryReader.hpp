//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 02.11.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>

namespace Yson
{
    class BinaryReader
    {
    public:
        virtual ~BinaryReader() = default;

        virtual bool advance(size_t count) = 0;

        virtual const void* data() const = 0;

        virtual char front() const = 0;

        virtual bool peek(char* value) = 0;

        virtual size_t position() const = 0;

        virtual bool read(size_t size) = 0;

        virtual bool read(void* buffer, size_t size, size_t unitSize = 1) = 0;

        virtual size_t size() = 0;
    protected:
        BinaryReader() = default;
    };
}
