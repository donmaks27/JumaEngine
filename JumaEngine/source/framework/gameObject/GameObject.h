// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
	class GameObject : public EngineContextObject
	{
	public:
		GameObject() = default;
		virtual ~GameObject() override = default;
	};
}
