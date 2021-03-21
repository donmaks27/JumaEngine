// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Rotation.h"
#include "utils/math_functions.h"
#include "glm/gtx/quaternion.hpp"

namespace JumaEngine
{
	glm::vec3 Rotation::toEuler() const
	{
		return glm::vec3(roll, pitch, yaw);
	}
	void Rotation::fromEuler(const glm::vec3& euler)
	{
		roll = euler.x;
		pitch = euler.y;
		yaw = euler.z;
	}

	glm::quat Rotation::toQuat() const
	{
		return glm::quat(glm::radians(glm::vec3(roll, pitch, -yaw)));
	}
	void Rotation::fromQuat(const glm::quat& quat)
	{
		pitch = glm::degrees(glm::yaw(quat));
		yaw = -glm::degrees(glm::roll(quat));
		roll = glm::degrees(glm::pitch(quat));
	}

	glm::vec3 Rotation::toDirection() const
	{
		return Consts::VectorForward * toQuat();
	}
	void Rotation::fromDirection(const glm::vec3& direction, const bool resetRoll)
	{
		if (resetRoll)
		{
			roll = 0.0f;
		}
		
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
