// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"
#include "Engine.h"
#include "framework/mesh/Mesh.h"
#include "render/vertexBuffer/VertexBufferData.h"

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
    	static RenderManagerBase* getRenderManager(const EngineContextObject* engineContextObject);
    	static WindowBase* getWindow(const EngineContextObject* engineContextObject);
    	static float getWindowAspectRatio(const EngineContextObject* engineContextObject);
    	
        static ShaderBase* createShader(const EngineContextObject* engineContextObject);
        static ShaderBase* createShader(const EngineContextObject* engineContextObject, const std::string& shaderName);
        static VertexBufferBase* createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData = nullptr);

        static VertexBufferImporterBase* getVertexBufferImporter(const EngineContextObject* engineContextObject);
        static void importVertexBufferFile(const EngineContextObject* engineContextObject, const char* filePath);
        template<typename T, typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<Mesh, T> && is_base_and_not_abstract<VertexBufferDataBase, U>)>
        static Mesh* importMesh(const EngineContextObject* engineContextObject, const std::string& meshName)
        {
            VertexBufferImporterBase* importer = getVertexBufferImporter(engineContextObject);
            if (importer != nullptr)
            {
                Engine* engine = getEngine(engineContextObject);
                Mesh* mesh = engine != nullptr ? engine->createObject<T>() : nullptr;
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
