// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class GlobalMaterialParamsSubsystem;
    class AsyncTasksSubsystem;
    class RenderTarget;
    class Texture;
    struct RenderOptions;
    class Shader;
    class Material;
    class VertexBuffer;
    class WindowSubsystem;
    class RenderSubsystem;

    class Engine final
    {
    public:
        Engine() = default;
        ~Engine() = default;

        EngineContextObject* createObject(const EngineContextObject::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return EngineContextObject::cast<T>(createObject(T::getClass())); }

        bool startEngine();
        
        void render(const RenderOptions* options);

        AsyncTasksSubsystem* getAsyncTasksSubsystem() const { return m_AsyncTasksSubsystem; }
        GlobalMaterialParamsSubsystem* getGlobalMaterialParamsSubsystem() const { return m_GlobalMaterialParamsSubsystem; }

        WindowSubsystem* getWindowSubsystem() const { return m_WindowSubsytem; }
        RenderSubsystem* getRenderSubsystem() const { return m_RenderSubsystem; }

    private:

        bool m_Started = false;

        AsyncTasksSubsystem* m_AsyncTasksSubsystem = nullptr;
        GlobalMaterialParamsSubsystem* m_GlobalMaterialParamsSubsystem = nullptr;

        WindowSubsystem* m_WindowSubsytem = nullptr;
        RenderSubsystem* m_RenderSubsystem = nullptr;

        Texture* m_Texture = nullptr;
        Shader* m_Shader = nullptr;
        Material* m_Material = nullptr;
        VertexBuffer* m_VertexBuffer = nullptr;

        RenderTarget* m_RenderTarget = nullptr;
        RenderTarget* m_WindowRenderTarget = nullptr;
        RenderTarget* m_SecondWindowRenderTarget = nullptr;

        Shader* m_ShaderPP = nullptr;
        Material* m_MaterialPP = nullptr;
        VertexBuffer* m_VertexBufferPP = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal();
        bool initEngine();

        void startEngineLoop();

        void terminate();
    };
}
