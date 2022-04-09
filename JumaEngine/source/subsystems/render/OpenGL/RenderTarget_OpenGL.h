// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderTarget.h"

namespace JumaEngine
{
    class RenderTarget_RenderAPIObject_OpenGL : public RenderTarget_RenderAPIObject
    {
    public:
        RenderTarget_RenderAPIObject_OpenGL() = default;
        virtual ~RenderTarget_RenderAPIObject_OpenGL() override;

        uint32 getColorImageIndex() const { return m_ColorImageIndex; }

        void startRender();
        void finishRender();

    protected:

        virtual bool initInternal() override;

    private:

        uint32 m_FramebufferIndex = 0;
        uint32 m_ColorImageIndex = 0;
        uint32 m_DepthRenderbufferIndex = 0;


        void clearData();
    };
}

#endif
