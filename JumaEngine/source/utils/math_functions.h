// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "math_consts.h"
#include "glm/gtc/epsilon.hpp"
#include "glm/trigonometric.hpp"

namespace JumaEngine
{
    class MathFunctions final
    {
    public:

        template<typename T>
        static T abs(T value) { return value > 0 ? value : -value; }
        template<typename T>
        static T max(T a, T b) { return a > b ? a : b; }
        template<typename T>
        static T min(T a, T b) { return a < b ? a : b; }
        template<typename T>
        static T clamp(T value, T min, T max) { return min(max(value, min), max); }
        
        template<typename T>
        static bool isNearlyEquals(T a, T b) { return isNearlyEquals(a, b, Consts::SmallNumber); }
        template<typename T>
        static bool isNearlyEquals(T a, T b, const float epsilon) { return abs(a - b) <= epsilon; }
        template<glm::length_t L, typename T, glm::qualifier Q>
        static bool isNearlyEquals(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b, const float epsilon)
        {
            return glm::all(glm::epsilonEqual(a, b, epsilon));
        }

    	static float degreesToRadians(const float degrees) { return glm::radians(degrees); }
    	static float radiansToDegrees(const float radians) { return glm::degrees(radians); }
    };
}
