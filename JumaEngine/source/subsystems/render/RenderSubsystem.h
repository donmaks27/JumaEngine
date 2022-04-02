// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "RenderAPIObject.h"

#include "RenderAPI.h"
#include "RenderPresentMode.h"
#include "subsystems/window/WindowID.h"

namespace JumaEngine
{
    class ShaderRenderAPIObject;
    class MaterialRenderAPIObject;
    class VertexBufferRenderAPIObject;
    class TextureRenderAPIObject;
    class RenderSubsystem;

    class RenderSubsystem_RenderAPIObject : public RenderAPIObject<RenderSubsystem>
    {
        friend RenderSubsystem;

    public:
        RenderSubsystem_RenderAPIObject() = default;
        virtual ~RenderSubsystem_RenderAPIObject() override = default;

        RenderSubsystem* getRenderSubsystem() const { return m_Parent; }

    protected:

        virtual bool initInternal() override { return true; }

        bool createMainWindow();
        void destroyMainWindow();

        virtual ShaderRenderAPIObject* createShaderObject() = 0;
        virtual MaterialRenderAPIObject* createMaterialObject() = 0;
        virtual VertexBufferRenderAPIObject* createVertexBufferObject() = 0;
        virtual TextureRenderAPIObject* createTextureObject() = 0;

        virtual void render() = 0;
        virtual void waitForRenderFinish() {}
    };

    class RenderSubsystem final : public SubsystemBase, public RenderAPIWrapperBase<RenderSubsystem_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

        friend RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

        RenderAPI getRenderAPI() const { return m_CurrentRenderAPI; }
        RenderPresentMode getPresentMode() const { return m_CurrentPresentMode; }

        window_id_type getMainWindowID() const { return m_MainWindowID; }
        bool shouldCloseMainWindow() const;

        ShaderRenderAPIObject* createShaderObject();
        MaterialRenderAPIObject* createMaterialObject();
        VertexBufferRenderAPIObject* createVertexBufferObject();
        TextureRenderAPIObject* createTextureObject();

        void render();
        void waitForRenderFinish();

    protected:

        virtual RenderSubsystem_RenderAPIObject* createRenderAPIObjectInternal() override;
        
        virtual void clearSubsystemInternal() override;

    private:

        RenderAPI m_CurrentRenderAPI = RenderAPI::Vulkan;
        RenderPresentMode m_CurrentPresentMode = RenderPresentMode::VSYNC;

        window_id_type m_MainWindowID = INVALID_WINDOW_ID;


        bool createMainWindow();
        void destroyMainWindow();
    };
}
