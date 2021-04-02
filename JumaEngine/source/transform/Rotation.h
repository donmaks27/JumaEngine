// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/gtc/quaternion.hpp"

namespace JumaEngine
{
	struct Rotation
	{
		Rotation()
			: Rotation(0.0f, 0.0f)
		{}
		Rotation(float p, float y, float r = 0.0f)
			: pitch(p)
			, yaw(y)
			, roll(r)
		{}
		Rotation(const Rotation& r) = default;

		Rotation& operator=(const Rotation& r) = default;
		Rotation& operator=(Rotation&& r) = default;
		
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

		glm::quat toQuat() const;
		void fromQuat(const glm::quat& quat);

		glm::vec3 toDirection() const;
		void fromDirection(const glm::vec3& direction, bool resetRoll = true);
	};
}
