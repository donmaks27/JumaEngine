// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderTarget.h"
#include "OpenGLContextObject.h"

#include "subsystems/asyncTasks/ActionTask.h"

namespace JumaEngine
{
    class RenderTarget_RenderAPIObject_OpenGL : public RenderTarget_RenderAPIObject, public OpenGLContextObject
    {
    public:
        RenderTarget_RenderAPIObject_OpenGL() = default;
        virtual ~RenderTarget_RenderAPIObject_OpenGL() override;
        
        window_id_type getUsingWindowID() const;
        uint32 getColorImageIndex() const { return m_ResolveColorAttachmentIndex == 0 ? m_ColorAttachmentIndex : m_ResolveColorAttachmentIndex; }

        bool startRender();
        void finishRender();

    protected:

        virtual bool initInternal() override;

        virtual bool shouldBeFlushed() const override { return true; }
        virtual void flushChanges() override;

    private:

        struct FramebufferIndices
        {
            bool valid = false;
            uint32 framebuffer = 0;
            uint32 resolveFramebuffer = 0;
        };

        uint32 m_ColorAttachmentIndex = 0;
        uint32 m_DepthAttachmentIndex = 0;
        uint32 m_ResolveColorAttachmentIndex = 0;

        FramebufferIndices m_Framebuffers;
        const ActionTaskResult<FramebufferIndices>* m_Framebuffers_CreateTask = nullptr;


        bool createAttachments();
        static FramebufferIndices createFramebuffers(uint32 colorAttachment, uint32 depthAttachment, uint32 resolveAttachment, 
            bool shouldResolveMultisampling);

        void clearData();
        static void clearFramebuffers(FramebufferIndices framebuffers);
    };
}

#endif
