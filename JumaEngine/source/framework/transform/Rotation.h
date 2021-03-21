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

		glm::vec3 toEuler() const { return glm::vec3(roll, yaw, pitch); }
		void fromEuler(const glm::vec3& euler)
		{
			pitch = euler.z;
			yaw = euler.y;
			roll = euler.x;
		}
		
		glm::quat toQuat() const { return glm::quat(glm::radians(toEuler())); }
		void fromQuat(const glm::quat& quat)
		{
			pitch = glm::degrees(glm::pitch(quat));
			yaw = glm::degrees(glm::yaw(quat));
			roll = glm::degrees(glm::roll(quat));
		}

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
	};
}
