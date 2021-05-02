// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "Engine.h"
#include "EngineContextObject.h"
#include "render/RenderManagerBase.h"
#include "render/vertexBuffer/importer/MeshImporterBase.h"

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

    MeshImporterBase* SystemFunctions::getVertexBufferImporter(const EngineContextObject* engineContextObject)
    {
        const Engine* engine = getEngine(engineContextObject);
        return engine != nullptr ? engine->getVertexBufferImporter() : nullptr;
    }

    CameraComponent* SystemFunctions::getWindowActiveCamera(const EngineContextObject* engineContextObject, const window_id windowID)
    {
        const RenderManagerBase* renderManager = getRenderManager(engineContextObject);
        return renderManager != nullptr ? renderManager->getWindowActiveCamera(windowID) : nullptr;
    }
}
