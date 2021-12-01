// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "TransformObject.h"
#include "engine/math_functions.h"

namespace JumaEngine
{
	Transform TransformObject::getWorldTransform() const
	{
		return m_Transform;
	}
	glm::vec3 TransformObject::getWorldLocatoin() const
	{
		return m_Transform.location;
	}
	Rotation TransformObject::getWorldRotation() const
	{
		return m_Transform.rotation;
	}
	glm::vec3 TransformObject::getWorldScale() const
	{
		return m_Transform.scale;
	}

	void TransformObject::setWorldTransform(const Transform& transform)
	{
		m_Transform = transform;
		onTransformChanged();
	}
	void TransformObject::setWorldLocation(const glm::vec3& location)
	{
		m_Transform.location = location;
		onTransformChanged();
	}
	void TransformObject::setWorldRotation(const Rotation& rotation)
	{
		m_Transform.rotation = rotation;
		onTransformChanged();
	}
	void TransformObject::setWorldScale(const glm::vec3& scale)
	{
		m_Transform.scale = scale;
		onTransformChanged();
	}
}