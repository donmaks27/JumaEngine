// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "Engine.h"
#include "EngineContextObject.h"
#include "render/RenderManagerBase.h"
#include "render/shader/ShaderBase.h"
#include "render/vertexBuffer/VertexBufferBase.h"
#include "render/vertexBuffer/importer/MeshImporterBase.h"
#include "asset/mesh/Mesh.h"

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

    MeshImporterBase* SystemFunctions::getVertexBufferImporter(const EngineContextObject* engineContextObject)
    {
        const Engine* engine = getEngine(engineContextObject);
        return engine != nullptr ? engine->getVertexBufferImporter() : nullptr;
    }
    void SystemFunctions::importVertexBufferFile(const EngineContextObject* engineContextObject, const char* filePath)
    {
        MeshImporterBase* importer = getVertexBufferImporter(engineContextObject);
        if (importer != nullptr)
        {
            importer->importFile(filePath);
        }
    }
    Mesh* SystemFunctions::importMesh(const EngineContextObject* engineContextObject, const jstring& meshName, const subclass<Mesh>& meshClass, 
        const subclass<VertexBufferDataBase>& bufferClass)
    {
        MeshImporterBase* importer = getVertexBufferImporter(engineContextObject);
        if (importer != nullptr)
        {
            Mesh* mesh = createObject<Mesh>(engineContextObject, meshClass);
            if (mesh != nullptr)
            {
                mesh->initMesh(importer->createVertexBuffersForMesh(meshName, bufferClass));
                return mesh;
            }
        }
        return nullptr;
    }

    CameraComponent* SystemFunctions::getWindowActiveCamera(const EngineContextObject* engineContextObject)
    {
        const RenderManagerBase* renderManager = getRenderManager(engineContextObject);
        return renderManager != nullptr ? renderManager->getWindowActiveCamera() : nullptr;
    }
}
