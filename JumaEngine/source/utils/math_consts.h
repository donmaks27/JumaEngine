// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec3.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace JumaEngine
{
	namespace Consts
	{
		constexpr float SmallNumber = 0.0001f;
		constexpr float PI = glm::pi<float>();
		
		constexpr glm::vec3 VectorForward = { 1.0f, 0.0f, 0.0f };
		constexpr glm::vec3 VectorRight = { 0.0f, 0.0f, 1.0f };
		constexpr glm::vec3 VectorUp = { 0.0f, 1.0f, 0.0f };
	}
	
}
