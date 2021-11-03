// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/VertexBuffer.h"

namespace JumaEngine
{
    class VertexBuffer_OpenGL final : public VertexBuffer
    {
        JUMAENGINE_CLASS(VertexBuffer_OpenGL, VertexBuffer)

    public:
        VertexBuffer_OpenGL() = default;
        virtual ~VertexBuffer_OpenGL() override;
        
        void render() const;

    protected:

        virtual bool initInternal(const VertexBufferDataBase* data) override;
        virtual void clearInternal() override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;


        void clearOpenGLBuffers();
    };
}

#endif
