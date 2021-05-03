// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <thread>
#include "EngineContextObject.h"
#include "utils/subclass.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class RenderManagerBase;
    class AssetsManager;
	class WindowBaseOld;
    class MeshImporterBase;

    class EngineWorld;
    class Mesh;
    class RenderTargetDirectBase;

    class Engine
    {
    public:
        Engine();
        virtual ~Engine() = default;

        bool isMainThread() const { return std::this_thread::get_id() == m_MainThreadID; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<MeshImporterBase, T>)>
        void setVertexBufferImporter()
        {
            if (m_MeshImporter == nullptr)
            {
                m_MeshImporter = new T();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        MeshImporterBase* getVertexBufferImporter() const { return m_MeshImporter; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject()
        {
        	T* object = new T();
	        registerEngineObject(object);
        	return object;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject(const subclass<T>& objectClass)
        {
        	T* object = objectClass.createObject();
	        registerEngineObject(object);
        	return object;
        }

        bool startEngine(int argc, char** argv);
        bool startEngine() { return startEngine(0, nullptr); }
    	
        bool shouldStopEngine() const;

        EngineWorld* getActiveWorld() const { return m_World; }

    private:

        std::thread::id m_MainThreadID;

		bool m_EngineStarted = false;
    	
        RenderManagerBase* m_RenderManager = nullptr;
        MeshImporterBase* m_MeshImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;

        Mesh* m_Mesh = nullptr;

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
