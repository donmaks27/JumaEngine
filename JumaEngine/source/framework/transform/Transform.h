// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Location.h"
#include "Rotation.h"
#include "Scale.h"

namespace JumaEngine
{
	struct Transform
	{
		Location location;
		Rotation rotation;
		Scale scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 toMatrix() const
		{
			glm::mat4 result = glm::mat4_cast(rotation.toQuat());
			result = glm::translate(result, location);
			return glm::scale(result, scale);
		}
	};
}
