// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/VertexBuffer.h"

namespace JumaEngine
{
    class VertexBufferObject_OpenGL : public VertexBufferObject
    {
    public:
        VertexBufferObject_OpenGL() = default;
        virtual ~VertexBufferObject_OpenGL() override;

        virtual bool render(const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;
        virtual void clearInternal() override { clearOpenGL(); }

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;


        void clearOpenGL();
    };
}

#endif
