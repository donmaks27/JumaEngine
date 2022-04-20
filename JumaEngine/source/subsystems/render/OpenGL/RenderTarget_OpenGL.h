// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderTarget.h"
#include "OpenGLContextObject.h"

namespace JumaEngine
{
    class RenderTarget_RenderAPIObject_OpenGL : public RenderTarget_RenderAPIObject, public OpenGLContextObject
    {
    public:
        RenderTarget_RenderAPIObject_OpenGL() = default;
        virtual ~RenderTarget_RenderAPIObject_OpenGL() override;

        uint32 getColorImageIndex() const { return m_ResolveColorAttachmentIndex == 0 ? m_ColorAttachmentIndex : m_ResolveColorAttachmentIndex; }

        void startRender();
        void finishRender();

    protected:

        virtual bool initInternal() override;

    private:

        uint32 m_FramebufferIndex = 0;
        uint32 m_ColorAttachmentIndex = 0;
        uint32 m_DepthAttachmentIndex = 0;

        uint32 m_ResolveFramebufferIndex = 0;
        uint32 m_ResolveColorAttachmentIndex = 0;


        void clearData();
    };
}

#endif
