// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "Texture_OpenGL.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    RenderTarget_RenderAPIObject_OpenGL::~RenderTarget_RenderAPIObject_OpenGL()
    {
        clearData();
    }

    bool RenderTarget_RenderAPIObject_OpenGL::initInternal()
    {
        // TODO: Create it in window thread
        const bool depthEnabled = true;
        const bool renderToWindow = m_Parent->isWindowRenderTarget();
        const bool shouldResolveMultisampling = m_Parent->shouldResolveMultisampling();
        const bool resolveFramebufferEnabled = shouldResolveMultisampling && !renderToWindow;
        if (!resolveFramebufferEnabled && renderToWindow)
        {
            m_FramebufferIndex = 0;
            m_ColorAttachmentIndex = 0;
            m_DepthAttachmentIndex = 0;
            m_ResolveFramebufferIndex = 0;
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
        const uint8 samplesNumber = GetTextureSamplesNumber(m_Parent->getTextureSamples());
        const int8 framebufferCount = resolveFramebufferEnabled ? 2 : 1;

        GLuint framebufferIndices[2] = { 0, 0 };
        GLuint colorAttachment = 0, depthAttachment = 0, resolveAttachment = 0;
        glGenFramebuffers(framebufferCount, framebufferIndices);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[0]);

        if (shouldResolveMultisampling)
        {
            glGenRenderbuffers(1, &colorAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, colorAttachment);
            if (samplesNumber == 1)
            {
                glRenderbufferStorage(GL_RENDERBUFFER, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
            }
            else
            {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                    static_cast<GLsizei>(samplesNumber), formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
                );
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorAttachment);
        }
        else
        {
            glGenTextures(1, &colorAttachment);
            if (samplesNumber == 1)
            {
                glBindTexture(GL_TEXTURE_2D, colorAttachment);
                glTexImage2D(GL_TEXTURE_2D, 
                    0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                    GL_RGB, GL_UNSIGNED_BYTE, nullptr
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                    static_cast<GLsizei>(samplesNumber), formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), GL_TRUE
                );
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorAttachment, 0);
            }
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
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);
        }

        if (resolveFramebufferEnabled)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[1]);
            glGenTextures(1, &resolveAttachment);
            glBindTexture(GL_TEXTURE_2D, resolveAttachment);
            glTexImage2D(GL_TEXTURE_2D, 
                0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                GL_RGB, GL_UNSIGNED_BYTE, nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveAttachment, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_FramebufferIndex = framebufferIndices[0];
        m_ColorAttachmentIndex = colorAttachment;
        m_DepthAttachmentIndex = depthAttachment;
        m_ResolveFramebufferIndex = framebufferIndices[1];
        m_ResolveColorAttachmentIndex = resolveAttachment;
        return true;
    }

    void RenderTarget_RenderAPIObject_OpenGL::clearData()
    {
        if (m_FramebufferIndex != 0)
        {
            glDeleteFramebuffers(1, &m_FramebufferIndex);
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
            if (m_ResolveFramebufferIndex != 0)
            {
                glDeleteFramebuffers(1, &m_ResolveFramebufferIndex);
                glDeleteTextures(1, &m_ResolveColorAttachmentIndex);
            }

            m_FramebufferIndex = 0;
            m_ColorAttachmentIndex = 0;
            m_DepthAttachmentIndex = 0;
            m_ResolveFramebufferIndex = 0;
            m_ResolveColorAttachmentIndex = 0;
        }
    }

    void RenderTarget_RenderAPIObject_OpenGL::startRender()
    {
        if (m_Parent->isWindowRenderTarget())
        {
            m_Parent->getOwnerEngine()->getWindowSubsystem()->onStartWindowRender(m_Parent->getWindowID());
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferIndex);
    }
    void RenderTarget_RenderAPIObject_OpenGL::finishRender()
    {
        if (m_Parent->shouldResolveMultisampling())
        {
            const math::uvector2& size = m_Parent->getSize();

            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferIndex);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveFramebufferIndex);
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
