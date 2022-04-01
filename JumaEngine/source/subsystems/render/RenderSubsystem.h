// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "RenderAPI.h"
#include "RenderPresentMode.h"
#include "jutils/math/vector2.h"
#include "subsystems/window/WindowID.h"

namespace JumaEngine
{
    struct RenderOptions;
    class TextureRenderAPIObject;
    class VertexBufferRenderAPIObject;
    class MaterialRenderAPIObject;
    class ShaderRenderAPIObject;

    class RenderSubsystem : public SubsystemBase
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

        RenderAPI getRenderAPI() const { return RenderAPI::Vulkan; }

        bool shouldCloseMainWindow() const;
        math::uvector2 getMainWindowSize() const;

        virtual void render() = 0;

        virtual void onEnginePreTerminate() {}

        virtual ShaderRenderAPIObject* createShaderObject() = 0;
        virtual MaterialRenderAPIObject* createMaterialObject() = 0;
        virtual VertexBufferRenderAPIObject* createVertexBufferObject() = 0;
        virtual TextureRenderAPIObject* createTextureObject() = 0;

        RenderPresentMode getPresentMode() const { return m_CurrentPresentMode; }

    protected:

        window_id_type m_MainWindowID = INVALID_WINDOW_ID;

        RenderPresentMode m_CurrentPresentMode = RenderPresentMode::VSYNC;


        bool createMainWindow();
        void destroyMainWindow();

        void callEngineRender(const RenderOptions* options) const;
    };
}
