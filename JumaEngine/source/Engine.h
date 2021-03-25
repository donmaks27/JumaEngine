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

    class Camera;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine() = default;

        enum ExitCode
        {
            OK = 0,
            EmptyWindowObject = 1,
            FailWindowInit = 2,
            EmptyRenderManager = 3,
            FailRenderManagerInit = 4,
        };

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<WindowBase, T>)>
        void setWindow()
        {
            if (m_Window == nullptr)
            {
                m_Window = new T();
            }
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<RenderManagerBase, T>)>
        void setRenderManager()
        {
            if (m_RenderManager == nullptr)
            {
                m_RenderManager = new T();
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

        int32 startEngine(int argc, char** argv);
        int32 startEngine() { return startEngine(0, nullptr); }

        bool shouldStopEngine() const;
        double getDeltaTime() const;

        virtual void onInit();
        virtual void onUpdate(double deltaTime);
        virtual void onPostUpdate();
        virtual void onStop();

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject()
        {
            T* object = new T();
            registerEngineObject(object);
            return object;
        }

        Camera* getActiveCamera() const { return m_Camera; }

    private:

        WindowBase* m_Window = nullptr;
        RenderManagerBase* m_RenderManager = nullptr;
        VertexBufferImporterBase* m_VertexBufferImporter = nullptr;

        Camera* m_Camera = nullptr;


        int32 startEngineInternal(int argc, char** argv);

        bool initWindow(int32& resultCode) const;
        bool initRender(int32& resultCode);

        void terminateEngine();

        void registerEngineObject(EngineContextObject* object);
    };
}
