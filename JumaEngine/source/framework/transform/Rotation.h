// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/gtc/quaternion.hpp"

namespace JumaEngine
{
	struct Rotation
	{
		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;

		Rotation& operator+=(const Rotation& otherRotation)
		{
			pitch += otherRotation.pitch;
			yaw += otherRotation.yaw;
			roll += otherRotation.roll;
			return *this;
		}
		Rotation operator+(const Rotation& otherRotation) const { return Rotation(*this) += otherRotation; }
		Rotation& operator-=(const Rotation& otherRotation)
		{
			pitch -= otherRotation.pitch;
			yaw -= otherRotation.yaw;
			roll -= otherRotation.roll;
			return *this;
		}
		Rotation operator-(const Rotation& otherRotation) const { return Rotation(*this) -= otherRotation; }

		glm::vec3 toEuler() const;
		void fromEuler(const glm::vec3& euler);

		glm::quat toQuat() const;
		void fromQuat(const glm::quat& quat);

		glm::vec3 toDirection() const;
		void fromDirection(const glm::vec3& direction, bool resetRoll = true);
	};
}
