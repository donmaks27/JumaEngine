// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "GameObject.h"
#include "framework/transform/TransformObject.h"

namespace JumaEngine
{
	class SceneObject : public GameObject, public TransformObject
	{
	public:
		SceneObject() = default;
		virtual ~SceneObject() override = default;
	
	protected:
		
		TransformObject* getParentTransformObject() const override { return nullptr; }
	};
}
