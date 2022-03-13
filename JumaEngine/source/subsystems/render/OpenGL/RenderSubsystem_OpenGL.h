// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderSubsystem.h"

namespace JumaEngine
{
    class RenderSubsystem_OpenGL : public RenderSubsystem
    {
        JUMAENGINE_CLASS(RenderSubsystem_OpenGL, RenderSubsystem)

    public:
        RenderSubsystem_OpenGL() = default;
        virtual ~RenderSubsystem_OpenGL() override = default;

        virtual void render() override;

        virtual ShaderObject* createShaderObject() override;
        virtual MaterialObject* createMaterialObject() override;
        virtual VertexBufferObject* createVertexBufferObject() override;
        virtual TextureObject* createTextureObject() override;

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;
    };
}

#endif
