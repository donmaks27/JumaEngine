// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "render/vertexBuffer/VertexBufferBase.h"

#ifndef JUMAENGINE_SINGLE_WINDOW
#include "utils/jmutex_shared.h"
#include "utils/jmap.h"
#endif

namespace JumaEngine
{
    class VertexBuffer_OpenGL final : public VertexBufferBase
    {
        JUMAENGINE_CLASS(VertexBuffer_OpenGL, VertexBufferBase)

    public:
        VertexBuffer_OpenGL() = default;
        virtual ~VertexBuffer_OpenGL() override;

        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    protected:

        virtual bool initInternal(VertexBufferDataBase* vertexBufferData) override;

        virtual void clearWindowData(window_id windowID) override;
        virtual bool hasAnyWindowData() const override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;

#ifndef JUMAENGINE_SINGLE_WINDOW
        jmap<window_id, uint32> m_VerticesVAOs;
        mutable jmutex_shared m_VerticesVAOsMutex;
#else
        uint32 m_VerticesVAO = 0;
#endif


        void bindVAO(window_id windowID);

        void clearBuffers();
    };
}

#endif
