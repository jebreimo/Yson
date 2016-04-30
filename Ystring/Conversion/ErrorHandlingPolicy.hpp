//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-10
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

/** @file
  * @brief Defines the error handling policies available to the Conversion
  *     class.
  */

namespace Ystring { namespace Conversion
{
    /** @brief "Namespace" for the the ErrorHandlingPolicy enum values.
      */
    struct ErrorHandlingPolicy
    {
        /** @brief ErrorHandlingPolicy enum.
          */
        enum Type
        {
            /** @brief Replace invalid characters with a
              *     replacement character.
              */
            REPLACE,
            /** @brief Stop conversion when an invalid character
              *     is encountered.
              */
            STOP,
            /** @brief Throw an exception when an invalid character
              *     is encountered.
              */
            THROW,
            /** @brief Skip invalid characters like they don't exist.
              */
            SKIP
        };
    };

    /** @brief A convenient typedef for the ErrorHandlingPolicy.
      */
    typedef ErrorHandlingPolicy::Type ErrorHandlingPolicy_t;

}}
