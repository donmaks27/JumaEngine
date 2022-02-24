// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderSubsystem.h"

namespace JumaEngine
{
    class RenderSubsystem_OpenGL : public RenderSubsystem
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem_OpenGL, RenderSubsystem)

    public:
        RenderSubsystem_OpenGL() = default;
        virtual ~RenderSubsystem_OpenGL() override = default;

        virtual void render() override;

        virtual jshared_ptr<VertexBufferOld> createVertexBuffer() override;
        virtual jshared_ptr<Shader> createShader() override;
        virtual jshared_ptr<Material> createMaterial() override;
        virtual jshared_ptr<Image> createImage() override;
        virtual jshared_ptr<RenderPrimitive> createRenderPrimitive() override;

    protected:

        virtual bool initSubsystemInternal() override;
    };
}

#endif
