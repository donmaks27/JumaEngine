// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "RenderOptionsOld.h"
#include "subsystems/window/Window.h"
#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    struct RenderOptions;
    class VertexBufferObject;
    class RenderPrimitive;
    class Image;
    class VertexBufferOld;
    class MaterialOld;
    class ShaderOld;
    class ShaderObject;
    class MaterialObject;

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

        virtual jshared_ptr<VertexBufferOld> createVertexBuffer() = 0;
        virtual jshared_ptr<ShaderOld> createShaderOld() = 0;
        virtual jshared_ptr<MaterialOld> createMaterial() = 0;
        virtual jshared_ptr<Image> createImage() = 0;
        virtual jshared_ptr<RenderPrimitive> createRenderPrimitive() = 0;

    protected:

        Window* m_MainWindow = nullptr;


        bool createMainWindow();
        void terminateMainWindow();

        void callEngineRender(const RenderOptions* options) const;
    };
}
