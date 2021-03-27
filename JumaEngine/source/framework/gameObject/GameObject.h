// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineObject.h"
#include "SceneContextObject.h"

namespace JumaEngine
{
	class GameObject : public EngineObject, public SceneContextObject
	{
	public:
		GameObject() = default;
		virtual ~GameObject() override = default;
	};
}
