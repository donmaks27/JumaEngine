﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "subsystems/window/Window.h"
#include "RenderPresentMode.h"

namespace JumaEngine
{
    struct RenderOptions;
    class TextureObject;
    class VertexBufferObject;
    class MaterialObject;
    class ShaderObject;

    class RenderSubsystem : public SubsystemBase
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;
        
        Window* getMainWindow() const { return m_MainWindow; }
        bool shouldCloseMainWindow() const { return (m_MainWindow != nullptr) && m_MainWindow->shouldClose(); }

        math::uvector2 getMainWindowSize() const { return m_MainWindow != nullptr ? m_MainWindow->getSize() : math::uvector2(0); }

        virtual void render() = 0;

        virtual void onEnginePreTerminate() {}

        virtual ShaderObject* createShaderObject() = 0;
        virtual MaterialObject* createMaterialObject() = 0;
        virtual VertexBufferObject* createVertexBufferObject() = 0;
        virtual TextureObject* createTextureObject() = 0;

        RenderPresentMode getPresentMode() const { return m_CurrentPresentMode; }

    protected:

        Window* m_MainWindow = nullptr;

        RenderPresentMode m_CurrentPresentMode = RenderPresentMode::VSYNC;


        bool createMainWindow();
        void terminateMainWindow();

        void callEngineRender(const RenderOptions* options) const;
    };
}
