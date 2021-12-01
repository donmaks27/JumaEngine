// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "jmath.h"
#include "vector3.h"

namespace jutils
{
    namespace math
    {
        template<vector_size_type Size, typename Type1, typename Type2>
        constexpr bool isVectorsEqual(const math::vector<Size, Type1>& value1, const math::vector<Size, Type2>& value2, const float eps = EPSILON_DEFAULT)
        {
            for (vector_size_type i = 0; i < Size; i++)
            {
                if (!math::isEqual(static_cast<float>(value1[i]), static_cast<float>(value2[i]), eps))
                {
                    return false;
                }
            }
            return true;
        }

        template<typename Type>
        constexpr float lengthSqr(const vector<2, Type>& value) { return value.x * value.x + value.y * value.y; }
        template<typename Type>
        constexpr float lengthSqr(const vector<3, Type>& value) { return value.x * value.x + value.y * value.y + value.z * value.z; }
        template<typename Type>
        constexpr float lengthSqr(const vector<4, Type>& value) { return value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w; }
        template<vector_size_type Size, typename Type>
        constexpr float length(const vector<Size, Type>& value) { return math::sqrt(lengthSqr(value)); }

        template<vector_size_type Size>
        constexpr vector<Size, float> normal(const vector<Size, float>& value, const float eps = EPSILON_DEFAULT)
        {
            const float lenSqr = math::lengthSqr(value);
            return math::isEqual(lenSqr, 0.0f, eps) ? vector<Size, float>(0.0f) : value / math::sqrt(lenSqr);
        }

        template<typename Type1, typename Type2>
        constexpr Type1 dot(const vector<2, Type1>& value1, const vector<2, Type2>& value2)
        {
            return value1.x * static_cast<Type1>(value2.x) + value1.y * static_cast<Type1>(value2.y);
        }
        template<typename Type1, typename Type2>
        constexpr Type1 dot(const vector<3, Type1>& value1, const vector<3, Type2>& value2)
        {
            return value1.x * static_cast<Type1>(value2.x) + value1.y * static_cast<Type1>(value2.y) + value1.z * static_cast<Type1>(value2.z);
        }
        template<typename Type1, typename Type2>
        constexpr Type1 dot(const vector<4, Type1>& value1, const vector<4, Type2>& value2)
        {
            return value1.x * static_cast<Type1>(value2.x) + value1.y * static_cast<Type1>(value2.y) + value1.z * static_cast<Type1>(value2.z) + value1.w * static_cast<Type1>(value2.w);
        }

        constexpr vector3 cross(const vector3& value1, const vector3& value2)
        {
            return {
                value1.y * value2.z - value1.z * value2.y,
                value1.z * value2.x - value1.x * value2.z,
                value1.x * value2.y - value1.y * value2.x
            };
        }
    }
}
