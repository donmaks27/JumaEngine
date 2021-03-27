// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Engine.h"
#include "framework/mesh/Mesh.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

namespace JumaEngine
{
	class RenderManagerBase;
	class WindowBase;

    class ShaderBase;
    class VertexBufferBase;
    class VertexBufferImporterBase;

    class Camera;

    class SystemFunctions final
    {
    public:

		static Engine* getEngine(const EngineContextObject* engineContextObject);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        static T* createObject(const EngineContextObject* engineContextObject)
        {
            Engine* engine = getEngine(engineContextObject);
            if (engine != nullptr)
            {
                return engine->createObject<T>();
            }
            return nullptr;
        }

    	static RenderManagerBase* getRenderManager(const EngineContextObject* engineContextObject);

    	static WindowBase* getWindow(const EngineContextObject* engineContextObject);
    	static float getWindowAspectRatio(const EngineContextObject* engineContextObject);
    	
        static ShaderBase* createShader(const EngineContextObject* engineContextObject);
        static ShaderBase* createShader(const EngineContextObject* engineContextObject, const jstring& shaderName);
        static VertexBufferBase* createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData = nullptr);

        static VertexBufferImporterBase* getVertexBufferImporter(const EngineContextObject* engineContextObject);
        static void importVertexBufferFile(const EngineContextObject* engineContextObject, const char* filePath);
        template<typename T, typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<Mesh, T> && is_base_and_not_abstract<VertexBufferDataBase, U>)>
        static Mesh* importMesh(const EngineContextObject* engineContextObject, const jstring& meshName)
        {
            VertexBufferImporterBase* importer = getVertexBufferImporter(engineContextObject);
            if (importer != nullptr)
            {
                Mesh* mesh = createObject<T>(engineContextObject);
                if (mesh != nullptr)
                {
                    mesh->initMesh(importer->createVertexBufferForMesh<U>(meshName));
                    return mesh;
                }
            }
            return nullptr;
        }

        static Camera* getActiveCamera(const EngineContextObject* engineContextObject);
    };
}
