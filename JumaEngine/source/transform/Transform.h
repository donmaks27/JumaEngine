// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Rotation.h"

namespace JumaEngine
{
	struct Transform
	{
		Transform() = default;
		Transform(const glm::vec3& l)
			: Transform(l, Rotation())
		{}
		Transform(const glm::vec3& l, const Rotation& r, const glm::vec3& s = glm::one<glm::vec3>())
			: location(l)
			, rotation(r)
			, scale(s)
		{}
		Transform(const Transform& t) = default;
		
		glm::vec3 location = { 0.0f, 0.0f, 0.0f };
		Rotation rotation;
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 toMatrix() const
		{
			glm::mat4 result = glm::translate(glm::identity<glm::mat4>(), location);
			result *= glm::mat4_cast(rotation.toQuat());
			return glm::scale(result, scale);
		}
	};
}
