// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "VertexBufferBase.h"

namespace JumaEngine
{
    class VertexBufferOpenGL final : public VertexBufferBase
    {
    public:
        VertexBufferOpenGL() = default;
        virtual ~VertexBufferOpenGL() override;

        virtual void init(VertexBufferDataBase* vertexBufferData) override;
        virtual bool isInit() const override { return m_VerticesVBO; }
        virtual void terminate() override;

        virtual void render() override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;

        uint32 m_VerticesCount = 0;
        uint32 m_IndicesCount = 0;


        void clearBuffers();
    };
}

#endif
