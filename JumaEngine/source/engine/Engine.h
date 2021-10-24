﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <thread>
#include "EngineContextObject.h"

namespace JumaEngine
{
    class RenderSubsystem;

    class Engine final
    {
    public:
        Engine() = default;
        ~Engine() = default;

        RenderSubsystem* getRenderSubsystem() const { return m_RenderSubsystem; }

        EngineContextObject* createObject(const EngineContextObject::ClassType* objectClass);

    private:

        RenderSubsystem* m_RenderSubsystem = nullptr;


        void registerEngineObject(EngineContextObject* object);
    };


    
    class RenderManagerBase;
    class AssetsManager;
    class MeshFileImporterBase;
    class TextureFileImporterBase;

    class EngineWorld;
    class RenderTargetDirectBase;

    class EngineOld
    {
    public:
        EngineOld();
        virtual ~EngineOld() = default;

        bool isMainThread() const { return std::this_thread::get_id() == m_MainThreadID; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<MeshFileImporterBase, T>)>
        void setMeshFileImporter()
        {
            if (m_MeshFileImporter == nullptr)
            {
                m_MeshFileImporter = createObject<T>();
            }
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<TextureFileImporterBase, T>)>
        void setTextureFileImporter()
        {
            if (m_TextureFileImporter == nullptr)
            {
                m_TextureFileImporter = createObject<T>();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        MeshFileImporterBase* getVertexBufferImporter() const { return m_MeshFileImporter; }

        EngineContextObjectOld* createObject(const EngineContextObjectOld::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObjectOld, T>)>
        T* createObject() { return dynamic_cast<T*>(createObject(T::getClass())); }

        bool startEngine(int argc, char** argv);
        bool startEngine() { return startEngine(0, nullptr); }
    	
        bool shouldStopEngine() const;

        EngineWorld* getActiveWorld() const { return m_World; }

    private:

        std::thread::id m_MainThreadID;

		bool m_EngineStarted = false;
    	
        RenderManagerBase* m_RenderManager = nullptr;
        MeshFileImporterBase* m_MeshFileImporter = nullptr;
        TextureFileImporterBase* m_TextureFileImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;

        RenderTargetDirectBase* m_RenderTarget = nullptr;


        void registerEngineObject(EngineContextObjectOld* object);

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
