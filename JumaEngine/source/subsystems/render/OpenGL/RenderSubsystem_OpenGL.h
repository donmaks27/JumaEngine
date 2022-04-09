// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderSubsystem.h"

namespace JumaEngine
{
    class RenderSubsystem_RenderAPIObject_OpenGL : public RenderSubsystem_RenderAPIObject
    {
        using Super = RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem_RenderAPIObject_OpenGL() = default;
        virtual ~RenderSubsystem_RenderAPIObject_OpenGL() override;

    protected:

        virtual bool initInternal() override;

        virtual Shader_RenderAPIObject* createShaderObject() override;
        virtual Material_RenderAPIObject* createMaterialObject() override;
        virtual VertexBuffer_RenderAPIObject* createVertexBufferObject() override;
        virtual Texture_RenderAPIObject* createTextureObject() override;
        virtual RenderTarget_RenderAPIObject* createRenderTargetObject() override;
        virtual RenderPipeline_RenderAPIObject* createRenderPipelineObject() override;

    private:

        void clearOpenGL();
    };
}

#endif
