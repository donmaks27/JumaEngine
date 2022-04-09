// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/VertexBuffer.h"

namespace JumaEngine
{
    class VertexBuffer_RenderAPIObject_OpenGL : public VertexBuffer_RenderAPIObject
    {
    public:
        VertexBuffer_RenderAPIObject_OpenGL() = default;
        virtual ~VertexBuffer_RenderAPIObject_OpenGL() override;
        
        virtual bool render(const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;


        void clearOpenGL();
    };
}

#endif
