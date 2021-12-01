// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../type_checks.h"

#include <cmath>

namespace jutils
{
    namespace math
    {
        constexpr float EPSILON_DEFAULT = 0.0001f;
        constexpr double EPSILON_DOUBLE_DEFAULT = 0.0001;

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
        constexpr T1 clamp(const T1& value, const T2& minValue, const T3& maxValue) { return math::min(math::max(value, minValue), maxValue); }
        template<typename T>
        constexpr const T& clamp(const T& value, const T& minValue, const T& maxValue) { return math::min(math::max(value, minValue), maxValue); }

        template<typename T>
        constexpr T sqr(const T& value) { return value * value; }
        inline float sqrt(const float value) { return ::sqrt(value); }

        inline float sin(const float angleRad) { return ::sin(angleRad); }
        inline float cos(const float angleRad) { return ::cos(angleRad); }
        inline float tan(const float angleRad) { return ::tan(angleRad); }
        inline float atan(const float angleRad) { return ::atan(angleRad); }

        template<typename T>
        constexpr T abs(const T& value) { return value < 0 ? -value : value; }

        constexpr bool isEqual(const float value1, const float value2, const float eps = EPSILON_DEFAULT) { return math::abs(value2 - value1) <= eps; }
        constexpr bool isEqual(const double value1, const double value2, const double eps = EPSILON_DOUBLE_DEFAULT) { return math::abs(value2 - value1) <= eps; }
    }
}
