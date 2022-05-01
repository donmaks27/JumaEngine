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
        
        window_id_type getUsingWindowID() const;
        uint32 getColorImageIndex() const { return m_FramebufferData.resolveColorAttachment == 0 ? m_FramebufferData.colorAttachment : m_FramebufferData.resolveColorAttachment; }

        bool startRender();
        void finishRender();

    protected:

        virtual bool initInternal() override;

    private:

        struct FramebufferData
        {
            uint32 colorAttachment = 0;
            uint32 depthAttachment = 0;
            uint32 resolveColorAttachment = 0;

            uint32 framebuffer = 0;
            uint32 resolveFramebuffer = 0;
        };

        FramebufferData m_FramebufferData;


        bool createFramebufferData();

        void clearData();
    };
}

#endif
