// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "math_consts.h"
#include "transform/Transform.h"

namespace JumaEngine
{
    class MathFunctions final
    {
    public:

        template<typename T>
        static bool isNearlyEquals(T a, T b) { return isNearlyEquals(a, b, Consts::SmallNumber); }
        template<typename T>
        static bool isNearlyEquals(T a, T b, const float epsilon) { return glm::epsilonEqual(a, b, epsilon); }
        template<glm::length_t L, typename T, glm::qualifier Q>
        static bool isNearlyEquals(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b, const float epsilon)
        {
            return glm::all(glm::epsilonEqual(a, b, epsilon));
        }
        static bool isNearlyEquals(const Rotation& a, const Rotation& b, const float epsilon)
        {
	        return isNearlyEquals(a.pitch, b.pitch, epsilon)
        		&& isNearlyEquals(a.yaw, b.yaw, epsilon)
        		&& isNearlyEquals(a.roll, b.roll, epsilon);
        }
        static bool isNearlyEquals(const Transform& a, const Transform& b, const float epsilon)
        {
	        return isNearlyEquals(a.location, b.location, epsilon)
        		&& isNearlyEquals(a.rotation, b.rotation, epsilon)
        		&& isNearlyEquals(a.scale, b.scale, epsilon);
        }
		
        template<typename T>
    	static bool isNearlyZero(T value) { return isNearlyZero(value, Consts::SmallNumber); }
        template<typename T>
    	static bool isNearlyZero(T value, const float epsilon) { return isNearlyEquals(value, 0.0f, epsilon); }
        template<glm::length_t L, typename T, glm::qualifier Q>
        static bool isNearlyZero(const glm::vec<L, T, Q>& v, const float epsilon)
        {
            for (int32 index = 0; index < L; index++)
            {
	            if (!isNearlyZero(v[index]))
	            {
		            return false;
	            }
            }
        	return true;
        }
    };
}
