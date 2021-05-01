// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
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

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine() = default;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<MeshImporterBase, T>)>
        void setVertexBufferImporter()
        {
            if (m_VertexBufferImporter == nullptr)
            {
                m_VertexBufferImporter = new T();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        MeshImporterBase* getVertexBufferImporter() const { return m_VertexBufferImporter; }

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
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject(const subclass<T>& objectClass)
        {
            T* object = objectClass.createObject();
            registerEngineObject(object);
            return object;
        }

        EngineWorld* getActiveWorld() const { return m_World; }

    private:

        RenderManagerBase* m_RenderManager = nullptr;
        MeshImporterBase* m_VertexBufferImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;

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
