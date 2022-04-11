// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "RenderAPIObject.h"

#include "RenderAPI.h"
#include "RenderPresentMode.h"
#include "texture/TextureSamples.h"

namespace JumaEngine
{
    class RenderPipeline;
    class RenderPipeline_RenderAPIObject;
    class RenderTarget_RenderAPIObject;
    class Shader_RenderAPIObject;
    class Material_RenderAPIObject;
    class VertexBuffer_RenderAPIObject;
    class Texture_RenderAPIObject;
    class RenderSubsystem;

    class RenderSubsystem_RenderAPIObject : public RenderAPIObject<RenderSubsystem>
    {
        friend RenderSubsystem;

    public:
        RenderSubsystem_RenderAPIObject() = default;
        virtual ~RenderSubsystem_RenderAPIObject() override = default;

    protected:

        virtual bool initInternal() override { return true; }

        virtual Shader_RenderAPIObject* createShaderObject() = 0;
        virtual Material_RenderAPIObject* createMaterialObject() = 0;
        virtual VertexBuffer_RenderAPIObject* createVertexBufferObject() = 0;
        virtual Texture_RenderAPIObject* createTextureObject() = 0;
        virtual RenderTarget_RenderAPIObject* createRenderTargetObject() = 0;
        virtual RenderPipeline_RenderAPIObject* createRenderPipelineObject() = 0;

        virtual void waitForRenderFinish() {}

        void clearData();
    };

    class RenderSubsystem final : public SubsystemBase, public RenderAPIWrapperBase<RenderSubsystem_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

        friend RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

        RenderAPI getRenderAPI() const { return m_CurrentRenderAPI; }
        TextureSamples getMaxTextureSamples() const { return m_MaxTextureSamples; }

        RenderPresentMode getPresentMode() const { return m_CurrentPresentMode; }

        RenderPipeline* getRenderPipeline() const { return m_RenderPipeline; }

        Shader_RenderAPIObject* createShaderObject();
        Material_RenderAPIObject* createMaterialObject();
        VertexBuffer_RenderAPIObject* createVertexBufferObject();
        Texture_RenderAPIObject* createTextureObject();
        RenderTarget_RenderAPIObject* createRenderTargetObject();
        RenderPipeline_RenderAPIObject* createRenderPipelineObject();

        void render();
        void waitForRenderFinish();

    protected:

        virtual RenderSubsystem_RenderAPIObject* createRenderAPIObjectInternal() override;
        
        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

    private:

        RenderAPI m_CurrentRenderAPI = RenderAPI::Vulkan;
        TextureSamples m_MaxTextureSamples = TextureSamples::SAMPLES_64;

        RenderPresentMode m_CurrentPresentMode = RenderPresentMode::VSYNC;

        RenderPipeline* m_RenderPipeline = nullptr;
    };
}
