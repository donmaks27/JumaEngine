// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class RenderManagerBase;
    class AssetsManager;
	class WindowBaseOld;
    class VertexBufferImporterBase;

    class EngineWorld;
    class CameraComponent;
    class Mesh;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine() = default;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<VertexBufferImporterBase, T>)>
        void setVertexBufferImporter()
        {
            if (m_VertexBufferImporter == nullptr)
            {
                m_VertexBufferImporter = new T();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        VertexBufferImporterBase* getVertexBufferImporter() const { return m_VertexBufferImporter; }

        bool startEngine(int argc, char** argv);
        bool startEngine() { return startEngine(0, nullptr); }
    	
        bool shouldStopEngine() const;
        double getDeltaTime() const;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject()
        {
            T* object = new T();
            registerEngineObject(object);
            return object;
        }

        EngineWorld* getActiveWorld() const { return m_World; }
        CameraComponent* getActiveCamera() const { return m_Camera; }

    private:

        RenderManagerBase* m_RenderManager = nullptr;
        VertexBufferImporterBase* m_VertexBufferImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;
        CameraComponent* m_Camera = nullptr;

        Mesh* m_Mesh = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal(int argc, char** argv);

        bool initEngine();
        bool initRender();

        void startEngineLoop();

        void terminateEngine();

        void onEngineInit();
    	void onGameStart();
        void tick(double deltaTime);
    	void postTick();
    	void render();
        void stopEngine();
    };
}
