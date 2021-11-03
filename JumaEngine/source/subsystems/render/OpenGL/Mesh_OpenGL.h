// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Mesh.h"

namespace JumaEngine
{
    class Mesh_OpenGL final : public Mesh
    {
        JUMAENGINE_CLASS(Mesh_OpenGL, Mesh)

    public:
        Mesh_OpenGL() = default;
        virtual ~Mesh_OpenGL() override = default;

        virtual void render(const RenderOptions& options) override;

    protected:

        virtual bool initInternal(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material) override { return true; }
        virtual void clearInternal() override {}
    };
}

#endif
