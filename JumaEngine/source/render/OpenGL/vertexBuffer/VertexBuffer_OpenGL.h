// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "utils/jmap.h"
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

        virtual void clearWindowData(window_id windowID) override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        jmap<window_id, uint32> m_VerticesVAOs;


        void bindVAO(window_id windowID);

        void clearBuffers();
    };
}

#endif
