// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "Engine.h"
#include "EngineContextObject.h"
#include "render/RenderManagerBase.h"
#include "render/vertexBuffer/VertexBufferBase.h"
#include "window/WindowBase.h"

namespace JumaEngine
{
	Engine* SystemFunctions::getEngine(const EngineContextObject* engineContextObject)
	{
		return engineContextObject != nullptr ? engineContextObject->getOwnerEngine() : nullptr;
	}
	RenderManagerBase* SystemFunctions::getRenderManager(const EngineContextObject* engineContextObject)
	{
		const Engine* engine = getEngine(engineContextObject);
		return engine != nullptr ? engine->getRenderManager() : nullptr;
	}
	WindowBase* SystemFunctions::getWindow(const EngineContextObject* engineContextObject)
	{
		const Engine* engine = getEngine(engineContextObject);
		return engine != nullptr ? engine->getWindow() : nullptr;
	}
	float SystemFunctions::getWindowAspectRatio(const EngineContextObject* engineContextObject)
	{
		const WindowBase* window = getWindow(engineContextObject);
		return window != nullptr ? window->getWindowAspectRatio() : 0.0f;
	}

	VertexBufferBase* SystemFunctions::createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData)
    {
		RenderManagerBase* renderManager = getRenderManager(engineContextObject);
		if (renderManager != nullptr)
		{
			VertexBufferBase* vertexBuffer = renderManager->createVertextBufferRender();
            if (vertexBufferData != nullptr)
            {
	            vertexBuffer->init(vertexBufferData);
            	return vertexBuffer;
            }
		}
		return nullptr;
    }
}
