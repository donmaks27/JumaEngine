// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "RenderOptions.h"
#include "subsystems/window/Window.h"
#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    class RenderPrimitive;
    class Image;
    class VertexBuffer;
    class Material;
    class Shader;

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

        virtual jshared_ptr<VertexBuffer> createVertexBuffer() = 0;
        virtual jshared_ptr<Shader> createShader() = 0;
        virtual jshared_ptr<Material> createMaterial() = 0;
        virtual jshared_ptr<Image> createImage() = 0;
        virtual jshared_ptr<RenderPrimitive> createRenderPrimitive() = 0;

    protected:

        Window* m_MainWindow = nullptr;


        bool createMainWindow();
        void terminateMainWindow();

        void callEngineRender(const RenderOptions& options) const;
    };
}
