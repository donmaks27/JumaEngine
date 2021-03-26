// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManagerBase.h"
#include <vector>

namespace JumaEngine
{
    class Material;
    class Mesh;

    class RenderManagerOpenGL : public RenderManagerBase
    {
    public:
        RenderManagerOpenGL() = default;
        virtual ~RenderManagerOpenGL() override = default;

        virtual bool init() override;

        virtual void render() override;

        virtual void terminate() override;

        virtual ShaderBase* createShader() override;
        virtual VertexBufferBase* createVertextBuffer() override;

    private:

        Material* m_Material = nullptr;
        Mesh* m_Mesh = nullptr;
    };
}
