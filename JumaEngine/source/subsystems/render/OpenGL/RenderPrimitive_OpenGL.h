// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderPrimitive.h"

namespace JumaEngine
{
    class RenderPrimitive_OpenGL final : public RenderPrimitive
    {
        JUMAENGINE_CLASS(RenderPrimitive_OpenGL, RenderPrimitive)

    public:
        RenderPrimitive_OpenGL() = default;
        virtual ~RenderPrimitive_OpenGL() override = default;

        virtual void render(const RenderOptionsOld& options) override;

    protected:

        virtual bool initInternal(const jshared_ptr<VertexBufferOld>& vertexBuffer, const jshared_ptr<Material>& material) override { return true; }
        virtual void clearInternal() override {}
    };
}

#endif
