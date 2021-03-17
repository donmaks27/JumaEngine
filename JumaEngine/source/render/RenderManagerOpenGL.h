// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManagerBase.h"

namespace JumaEngine
{
    class ShaderBase;

    class RenderManagerOpenGL : public RenderManagerBase
    {
    public:
        RenderManagerOpenGL();
        virtual ~RenderManagerOpenGL() override;

        virtual bool init() override;

        virtual void startFrameRender() override;

        virtual void terminate() override;

    private:

        ShaderBase* m_Shader = nullptr;
        float* m_Vertex = nullptr;
        unsigned int m_VertexVBO;
        unsigned int m_VertexVAO;
    };
}
