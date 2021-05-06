// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Engine.h"
#include "render/window/window_id.h"

namespace JumaEngine
{
    class MeshImporterBase;
    class CameraComponent;

    class SystemFunctions final
    {
    public:

		static Engine* getEngine(const EngineContextObject* engineContextObject) { return engineContextObject != nullptr ? engineContextObject->getOwnerEngine() : nullptr; }

        static MeshImporterBase* getVertexBufferImporter(const EngineContextObject* engineContextObject)
		{
		    const Engine* engine = getEngine(engineContextObject);
            return engine != nullptr ? engine->getVertexBufferImporter() : nullptr;
		}

        static CameraComponent* getWindowActiveCamera(const EngineContextObject* engineContextObject, window_id windowID);
    };
}
