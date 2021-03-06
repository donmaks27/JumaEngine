// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Rotation.h"
#include "utils/math_functions.h"
#include "glm/gtx/quaternion.hpp"

namespace JumaEngine
{
	glm::quat Rotation::toQuat() const
	{
		return glm::quat(glm::radians(glm::vec3(roll, -pitch, yaw)));
	}
	void Rotation::fromQuat(const glm::quat& quat)
	{
		roll = glm::degrees(glm::pitch(quat));
		pitch = glm::degrees(-glm::yaw(quat));
		yaw = glm::degrees(glm::roll(quat));
	}

	glm::vec3 Rotation::toDirection() const
	{
		return glm::rotate(toQuat(), Consts::VectorForward);
	}
	void Rotation::fromDirection(const glm::vec3& direction)
	{
		if (MathFunctions::isNearlyZero(direction))
		{
			pitch = 0.0f;
			yaw = 0.0f;
		}
		else
		{
			fromQuat(glm::rotation(Consts::VectorForward, glm::normalize(direction)));
		}
	}
}
