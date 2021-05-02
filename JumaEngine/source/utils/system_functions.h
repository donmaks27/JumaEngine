// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Engine.h"
#include "render/window/window_id.h"

namespace JumaEngine
{
	class RenderManagerBaseOld;
	class WindowBaseOld;

    class ShaderBase;
    class VertexBufferBase;
    class MeshImporterBase;
    class Mesh;
    class VertexBufferDataBase;

    class CameraComponent;

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
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        static T* createObject(const EngineContextObject* engineContextObject, const subclass<T>& objectClass)
        {
            Engine* engine = getEngine(engineContextObject);
            if (engine != nullptr)
            {
                return engine->createObject<T>(objectClass);
            }
            return nullptr;
        }

    	static RenderManagerBase* getRenderManager(const EngineContextObject* engineContextObject);
    	
        static ShaderBase* createShader(const EngineContextObject* engineContextObject);
        static ShaderBase* createShader(const EngineContextObject* engineContextObject, const jstring& shaderName);
        static VertexBufferBase* createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData = nullptr);

        static MeshImporterBase* getVertexBufferImporter(const EngineContextObject* engineContextObject);
        static void importVertexBufferFile(const EngineContextObject* engineContextObject, const char* filePath);
        static Mesh* importMesh(const EngineContextObject* engineContextObject, const jstring& meshName, const subclass<Mesh>& meshClass, 
            const subclass<VertexBufferDataBase>& bufferClass);

        static CameraComponent* getWindowActiveCamera(const EngineContextObject* engineContextObject, window_id windowID);
    };
}
