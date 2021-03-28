// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManagerBase.h"

namespace JumaEngine
{
    class Material;
    class Mesh;

    class RenderManagerOpenGL : public RenderManagerBase
    {
    public:
        RenderManagerOpenGL() = default;
        virtual ~RenderManagerOpenGL() override = default;

    	virtual RenderAPI getRenderAPI() const override { return RenderAPI::OpenGL; }

        virtual bool init() override;
        virtual void terminate() override;

        virtual void render() override;

        virtual ShaderBase* createShader() override;
        virtual VertexBufferBase* createVertextBuffer() override;
    };
}
