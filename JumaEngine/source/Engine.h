// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class WindowBase;
    class RenderManagerBase;
    class VertexBufferImporterBase;

    class EngineWorld;
    class CameraComponent;
    class Material;
    class Mesh;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine() = default;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<WindowBase, T>)>
        void setWindow()
        {
            if (m_Window == nullptr)
            {
                m_Window = createObject<T>();
            }
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<RenderManagerBase, T>)>
        void setRenderManager()
        {
            if (m_RenderManager == nullptr)
            {
                m_RenderManager = createObject<T>();
            }
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<VertexBufferImporterBase, T>)>
        void setVertexBufferImporter()
        {
            if (m_VertexBufferImporter == nullptr)
            {
                m_VertexBufferImporter = new T();
            }
        }

        WindowBase* getWindow() const { return m_Window; }
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

        WindowBase* m_Window = nullptr;
        RenderManagerBase* m_RenderManager = nullptr;
        VertexBufferImporterBase* m_VertexBufferImporter = nullptr;

        EngineWorld* m_World = nullptr;
        CameraComponent* m_Camera = nullptr;

        Material* m_Material = nullptr;
        Mesh* m_Mesh = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal(int argc, char** argv);

        bool initEngine();
        bool initWindow() const;
        bool initRender() const;

        void startEngineLoop();

        void terminateEngine();

        void onEngineInit();
        void tick(double deltaTime);
        void stopEngine();
    };
}
