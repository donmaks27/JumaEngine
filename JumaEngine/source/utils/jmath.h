// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "type_checks.h"

namespace jutils
{
    namespace math
    {
        template<typename T1, typename T2, TEMPLATE_ENABLE(is_castable<T2, T1>)>
        constexpr T1 max(const T1& value1, const T2& value2) { return value1 > value2 ? value1 : static_cast<T1>(value2); }
        template<typename T>
        constexpr const T& max(const T& value1, const T& value2) { return value1 > value2 ? value1 : value2; }

        template<typename T1, typename T2, TEMPLATE_ENABLE(is_castable<T2, T1>)>
        constexpr T1 min(const T1& value1, const T2& value2) { return value1 < value2 ? value1 : static_cast<T1>(value2); }
        template<typename T>
        constexpr const T& min(const T& value1, const T& value2) { return value1 < value2 ? value1 : value2; }
        
        template<typename T1, typename T2, typename T3>
        constexpr bool isWithin(const T1& value, const T2& minValue, const T3& maxValue) { return (value >= minValue) && (value <= maxValue); }

        template<typename T1, typename T2, typename T3, TEMPLATE_ENABLE(is_castable<T2, T1> && is_castable<T3, T1>)>
        constexpr T1 clamp(const T1& value, const T2& minValue, const T3& maxValue) { return min(max(value, minValue), maxValue); }
        template<typename T>
        constexpr const T& clamp(const T& value, const T& minValue, const T& maxValue) { return min(max(value, minValue), maxValue); }
    }
}
