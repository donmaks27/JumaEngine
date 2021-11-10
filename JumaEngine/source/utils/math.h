// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "utils/int_defines.h"

namespace jutils
{
    namespace math
    {
        template<typename T>
        inline constexpr const T& max(const T& value1, const T& value2) { return value1 > value2 ? value1 : value2; }
        template<typename T>
        inline constexpr T max(std::initializer_list<T> valueList)
        {
            const T* value = valueList.begin();
            const T* maxValue = value;
            if (value != valueList.end())
            {
                while (++value != valueList.end())
                {
                    if (value > maxValue)
                    {
                        maxValue = value;
                    }
                }
            }
            return value;
        }
    }
}
