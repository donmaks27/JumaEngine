// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "transform/Transform.h"

namespace JumaEngine
{
	class TransformObject
	{
	protected:
		TransformObject() = default;
		virtual ~TransformObject() = default;

	public:

		Transform getWorldTransform() const;
		glm::vec3 getWorldLocatoin() const;
		Rotation getWorldRotation() const;
		glm::vec3 getWorldScale() const;

		void setWorldTransform(const Transform& transform);
		void setWorldLocation(const glm::vec3& location);
		void setWorldRotation(const Rotation& rotation);
		void setWorldScale(const glm::vec3& scale);

	protected:

		virtual TransformObject* getParentTransformObject() const { return nullptr; }

		virtual void onTransformChanged() {}
	
	private:

		Transform m_Transform;
	};
}
