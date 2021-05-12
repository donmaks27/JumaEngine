// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <thread>
#include "EngineContextObject.h"

namespace JumaEngine
{
    class RenderManagerBase;
    class AssetsManager;
    class MeshFileImporterBase;

    class EngineWorld;
    class RenderTargetDirectBase;

    class Engine
    {
    public:
        Engine();
        virtual ~Engine() = default;

        bool isMainThread() const { return std::this_thread::get_id() == m_MainThreadID; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<MeshFileImporterBase, T>)>
        void setVertexBufferImporter()
        {
            if (m_MeshImporter == nullptr)
            {
                m_MeshImporter = createObject<T>();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        MeshFileImporterBase* getVertexBufferImporter() const { return m_MeshImporter; }

        EngineContextObject* createObject(const EngineContextObject::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject() { return dynamic_cast<T*>(createObject(T::getClass())); }

        bool startEngine(int argc, char** argv);
        bool startEngine() { return startEngine(0, nullptr); }
    	
        bool shouldStopEngine() const;

        EngineWorld* getActiveWorld() const { return m_World; }

    private:

        std::thread::id m_MainThreadID;

		bool m_EngineStarted = false;
    	
        RenderManagerBase* m_RenderManager = nullptr;
        MeshFileImporterBase* m_MeshImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;

        RenderTargetDirectBase* m_RenderTarget = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal(int argc, char** argv);

        bool initEngine();
        void onEngineInit();

        void startEngineLoop();
    	void onEngineLoopStart();

        void tick(double deltaTime);
    	void render();

        void stopEngine();
        void terminateEngine();
    };
}
