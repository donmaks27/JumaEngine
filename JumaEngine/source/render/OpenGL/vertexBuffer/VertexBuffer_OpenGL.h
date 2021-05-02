// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "render/vertexBuffer/VertexBufferBase.h"

namespace JumaEngine
{
    class VertexBuffer_OpenGL final : public VertexBufferBase
    {
    public:
        VertexBuffer_OpenGL() = default;
        virtual ~VertexBuffer_OpenGL() override;

        virtual void render(window_id windowID) override;

    protected:

        virtual bool initInternal(VertexBufferDataBase* vertexBufferData) override;
        virtual void terminateInteranl() override { clearBuffers(); }

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;


        void clearBuffers();
    };
}

#endif
