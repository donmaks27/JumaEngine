// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    //class EngineContextObject;
    class WindowBase;
    class RenderManagerBase;

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

        int32 startEngine(int argc, char** argv, WindowBase* window, RenderManagerBase* renderManager);
        int32 startEngine(WindowBase* window, RenderManagerBase* renderManager) { return startEngine(0, nullptr, window, renderManager); }

        WindowBase* getWindow() const { return m_Window; }
        RenderManagerBase* getRenderManager() const { return m_RenderManager; }

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

    private:

        WindowBase* m_Window = nullptr;
        RenderManagerBase* m_RenderManager = nullptr;


        int32 startEngineInternal(int argc, char** argv, WindowBase* window, RenderManagerBase* renderManager);

        bool initWindow(int32& resultCode, WindowBase* window);
        bool initRender(int32& resultCode, RenderManagerBase* renderManager);

        void terminateEngine();

        void registerEngineObject(EngineContextObject* object);
    };
}
