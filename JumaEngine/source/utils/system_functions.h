// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
	class Engine;
    class EngineContextObject;
	class RenderManagerBase;
	class WindowBase;
    class VertexBufferBase;
    class VertexBufferDataBase;

    class SystemFunctions final
    {
    public:

		static Engine* getEngine(const EngineContextObject* engineContextObject);
    	static RenderManagerBase* getRenderManager(const EngineContextObject* engineContextObject);
    	static WindowBase* getWindow(const EngineContextObject* engineContextObject);
    	static float getWindowAspectRatio(const EngineContextObject* engineContextObject);
    	
        static VertexBufferBase* createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData = nullptr);
    };
}
