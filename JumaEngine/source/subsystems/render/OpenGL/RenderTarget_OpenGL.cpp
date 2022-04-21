// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "Texture_OpenGL.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"
#include "subsystems/window/OpenGL/WindowSubsystem_OpenGL.h"

namespace JumaEngine
{
    RenderTarget_RenderAPIObject_OpenGL::~RenderTarget_RenderAPIObject_OpenGL()
    {
        clearData();
    }
    
    window_id_type RenderTarget_RenderAPIObject_OpenGL::getUsingWindowID() const
    {
        return m_Parent->isWindowRenderTarget() ? m_Parent->getWindowID() : getWindowSubsystemObject()->getParent()->getMainWindowID();
    }

    bool RenderTarget_RenderAPIObject_OpenGL::initInternal()
    {
        // Main thread
        if (!createAttachments())
        {
            JUMA_LOG(error, JSTR("Failed to create attachments for framebuffer"));
            return false;
        }
        if (!m_Framebuffers.valid)
        {
            ActionTask createTask;
            m_Framebuffers_CreateTask = createTask.bindRaw(&RenderTarget_RenderAPIObject_OpenGL::createFramebuffers,
                m_ColorAttachmentIndex, m_DepthAttachmentIndex, m_ResolveColorAttachmentIndex, 
                GetTextureSamplesNumber(m_Parent->getTextureSamples()) > 1
            );

            if (!getWindowSubsystemObject()->submitTaskForWindow(getUsingWindowID(), std::move(createTask)))
            {
                JUMA_LOG(error, JSTR("Failed to send task for creating framebuffers"));
                m_Framebuffers_CreateTask = nullptr;
                return false;
            }
        }
        return true;
    }
    bool RenderTarget_RenderAPIObject_OpenGL::createAttachments()
    {
        // Main thread
        const bool depthEnabled = true;
        const bool renderToWindow = m_Parent->isWindowRenderTarget();
        const uint8 samplesNumber = GetTextureSamplesNumber(m_Parent->getTextureSamples());
        const bool shouldResolveMultisampling = samplesNumber > 1;
        if (!shouldResolveMultisampling && renderToWindow)
        {
            m_Framebuffers = { true, 0, 0 };
            m_ColorAttachmentIndex = 0;
            m_DepthAttachmentIndex = 0;
            m_ResolveColorAttachmentIndex = 0;
            return true;
        }

        const TextureFormat format = !renderToWindow ? m_Parent->getFormat() : TextureFormat::RGBA;
        const uint32 formatOpenGL = GetOpenGLFormatByTextureFormat(format);
        if (formatOpenGL == 0)
        {
            JUMA_LOG(error, JSTR("Unsupported render target format"));
            return false;
        }
        
        const math::uvector2& size = m_Parent->getSize();
        GLuint colorAttachment = 0, depthAttachment = 0, resolveAttachment = 0;

        if (shouldResolveMultisampling)
        {
            glGenRenderbuffers(1, &colorAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, colorAttachment);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                static_cast<GLsizei>(samplesNumber), formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
            );
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        else
        {
            glGenTextures(1, &colorAttachment);
            glBindTexture(GL_TEXTURE_2D, colorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 
                0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                GL_RGB, GL_UNSIGNED_BYTE, nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (depthEnabled)
        {
            glGenRenderbuffers(1, &depthAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
            if (samplesNumber == 1)
            {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
            }
            else
            {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, 
                    static_cast<GLsizei>(samplesNumber), GL_DEPTH24_STENCIL8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
                );
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        if (shouldResolveMultisampling && !renderToWindow)
        {
            glGenTextures(1, &resolveAttachment);
            glBindTexture(GL_TEXTURE_2D, resolveAttachment);
            glTexImage2D(GL_TEXTURE_2D, 
                0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                GL_RGB, GL_UNSIGNED_BYTE, nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        m_ColorAttachmentIndex = colorAttachment;
        m_DepthAttachmentIndex = depthAttachment;
        m_ResolveColorAttachmentIndex = resolveAttachment;
        return true;
    }
    RenderTarget_RenderAPIObject_OpenGL::FramebufferIndices RenderTarget_RenderAPIObject_OpenGL::createFramebuffers(const uint32 colorAttachment, 
        const uint32 depthAttachment, const uint32 resolveAttachment, const bool shouldResolveMultisampling)
    {
        // Window thread
        const int8 framebufferCount = resolveAttachment != 0 ? 2 : 1;
        GLuint framebufferIndices[2] = { 0, 0 };
        glGenFramebuffers(framebufferCount, framebufferIndices);

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[0]);
        if (shouldResolveMultisampling)
        {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorAttachment);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
        }
        if (depthAttachment != 0)
        {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);
        }
        if (framebufferCount > 1)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[1]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveAttachment, 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return { true, framebufferIndices[0], framebufferIndices[1] };
    }

    void RenderTarget_RenderAPIObject_OpenGL::flushChanges()
    {
        // Main thread, after render
        if ((m_Framebuffers_CreateTask != nullptr) && m_Framebuffers_CreateTask->isTaskFinished())
        {
            m_Framebuffers = m_Framebuffers_CreateTask->get();
            m_Framebuffers_CreateTask->markAsHandled();
            m_Framebuffers_CreateTask = nullptr;
        }
    }

    void RenderTarget_RenderAPIObject_OpenGL::clearData()
    {
        // Main thread
        if (m_Framebuffers.valid && (m_Framebuffers.framebuffer != 0))
        {
            ActionTask clearTask(true);
            clearTask.bindRaw(&RenderTarget_RenderAPIObject_OpenGL::clearFramebuffers, m_Framebuffers);
            getWindowSubsystemObject()->submitTaskForWindow(getUsingWindowID(), std::move(clearTask));

            if (m_Parent->shouldResolveMultisampling())
            {
                glDeleteRenderbuffers(1, &m_ColorAttachmentIndex);
            }
            else
            {
                glDeleteTextures(1, &m_ColorAttachmentIndex);
            }
            if (m_DepthAttachmentIndex != 0)
            {
                glDeleteRenderbuffers(1, &m_DepthAttachmentIndex);
            }
            if (m_ResolveColorAttachmentIndex != 0)
            {
                glDeleteTextures(1, &m_ResolveColorAttachmentIndex);
            }

            m_Framebuffers.framebuffer = 0;
            m_ColorAttachmentIndex = 0;
            m_DepthAttachmentIndex = 0;
            m_Framebuffers.resolveFramebuffer = 0;
            m_ResolveColorAttachmentIndex = 0;
        }
    }
    void RenderTarget_RenderAPIObject_OpenGL::clearFramebuffers(const FramebufferIndices framebuffers)
    {
        // Window thread
        if (framebuffers.valid)
        {
            if (framebuffers.framebuffer != 0)
            {
                glDeleteFramebuffers(1, &framebuffers.framebuffer);
                if (framebuffers.resolveFramebuffer != 0)
                {
                    glDeleteFramebuffers(1, &framebuffers.resolveFramebuffer);
                }
            }
        }
    }

    bool RenderTarget_RenderAPIObject_OpenGL::startRender()
    {
        // Window thread
        if (!m_Framebuffers.valid)
        {
            return false;
        }

        if (m_Parent->isWindowRenderTarget())
        {
            getWindowSubsystemObject()->getParent()->onStartWindowRender(m_Parent->getWindowID());
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffers.framebuffer);
        return true;
    }
    void RenderTarget_RenderAPIObject_OpenGL::finishRender()
    {
        // Window thread
        if (m_Parent->shouldResolveMultisampling())
        {
            const math::uvector2& size = m_Parent->getSize();

            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffers.framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffers.resolveFramebuffer);
            glBlitFramebuffer(
                0, 0, static_cast<GLint>(size.x), static_cast<GLint>(size.y), 
                0, 0, static_cast<GLint>(size.x), static_cast<GLint>(size.y), 
                GL_COLOR_BUFFER_BIT, GL_NEAREST
            );
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (m_Parent->isWindowRenderTarget())
        {
            m_Parent->getOwnerEngine()->getWindowSubsystem()->onFinishWindowRender(m_Parent->getWindowID());
        }
    }
}

#endif
