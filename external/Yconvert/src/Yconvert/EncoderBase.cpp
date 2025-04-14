//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-27
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "EncoderBase.hpp"

namespace Yconvert
{

    EncoderBase::EncoderBase(Encoding encoding)
        : encoding_(encoding),
          error_policy_(),
          replacement_character_(REPLACEMENT_CHARACTER)
    {}

    Encoding EncoderBase::encoding() const
    {
        return encoding_;
    }

    ErrorPolicy EncoderBase::error_policy() const
    {
        return error_policy_;
    }

    void EncoderBase::set_error_policy(ErrorPolicy policy)
    {
        error_policy_ = policy;
    }

    char32_t EncoderBase::replacement_character() const
    {
        return replacement_character_;
    }

    void EncoderBase::set_replacement_character(char32_t value)
    {
        replacement_character_ = value;
    }
}
