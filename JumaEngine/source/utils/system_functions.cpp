// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "Engine.h"
#include "EngineContextObject.h"
#include "render/RenderManagerBase.h"
#include "render/shader/ShaderBase.h"
#include "render/vertexBuffer/VertexBufferBase.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

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

    ShaderBase* SystemFunctions::createShader(const EngineContextObject* engineContextObject)
    {
        RenderManagerBase* renderManager = getRenderManager(engineContextObject);
        if (renderManager != nullptr)
        {
            return renderManager->createShader();
        }
        return nullptr;
    }
    ShaderBase* SystemFunctions::createShader(const EngineContextObject* engineContextObject, const jstring& shaderName)
    {
        ShaderBase* shader = createShader(engineContextObject);
        if (shader != nullptr)
        {
            shader->loadShader(shaderName);
            return shader;
        }
        return nullptr;
    }

    VertexBufferBase* SystemFunctions::createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData)
    {
		RenderManagerBase* renderManager = getRenderManager(engineContextObject);
		if (renderManager != nullptr)
		{
			VertexBufferBase* vertexBuffer = renderManager->createVertextBuffer();
            if (vertexBufferData != nullptr)
            {
	            vertexBuffer->init(vertexBufferData);
            	return vertexBuffer;
            }
		}
		return nullptr;
    }

    VertexBufferImporterBase* SystemFunctions::getVertexBufferImporter(const EngineContextObject* engineContextObject)
    {
        const Engine* engine = getEngine(engineContextObject);
        return engine != nullptr ? engine->getVertexBufferImporter() : nullptr;
    }
    void SystemFunctions::importVertexBufferFile(const EngineContextObject* engineContextObject, const char* filePath)
    {
        VertexBufferImporterBase* importer = getVertexBufferImporter(engineContextObject);
        if (importer != nullptr)
        {
            importer->importFile(filePath);
        }
    }

    CameraComponent* SystemFunctions::getActiveCamera(const EngineContextObject* engineContextObject)
    {
        const Engine* engine = getEngine(engineContextObject);
        return engine != nullptr ? engine->getActiveCamera() : nullptr;
    }
}
