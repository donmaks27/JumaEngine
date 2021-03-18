// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertextBufferRenderBase.h"

namespace JumaEngine
{
    class VertextBufferRenderOpenGL final : public VertextBufferRenderBase
    {
    public:
        VertextBufferRenderOpenGL() = default;
        virtual ~VertextBufferRenderOpenGL() override;

        virtual void draw() override;

    protected:

        virtual bool initInternal() override;
        virtual void terminateInternal() override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        uint32 m_VerticesVAO = 0;


        void clearBuffers();
    };
}
