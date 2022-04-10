// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "Texture_OpenGL.h"

namespace JumaEngine
{
    RenderTarget_RenderAPIObject_OpenGL::~RenderTarget_RenderAPIObject_OpenGL()
    {
        clearData();
    }

    bool RenderTarget_RenderAPIObject_OpenGL::initInternal()
    {
        const TextureFormat format = m_Parent->getFormat();
        const uint32 formatOpenGL = GetOpenGLFormatByTextureFormat(format);
        if (formatOpenGL == 0)
        {
            JUMA_LOG(error, JSTR("Unsupported render target format"));
            return false;
        }

        const math::uvector2& size = m_Parent->getSize();
        const uint8 samplesNumber = GetTextureSamplesNumber(m_Parent->getTextureSamples());
        if (!m_Parent->shouldResolveMultisampling())
        {
            GLuint framebufferIndex = 0, colorAttachment = 0, depthAttachment = 0;
            glGenFramebuffers(1, &framebufferIndex);
            glGenTextures(1, &colorAttachment);
            glGenRenderbuffers(1, &depthAttachment);

            glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndex);

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

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                JUMA_LOG(error, JSTR("Failed to initialize framebuffer"));
                glDeleteFramebuffers(1, &framebufferIndex);
                glDeleteTextures(1, &colorAttachment);
                glDeleteRenderbuffers(1, &depthAttachment);
                return false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_FramebufferIndex = framebufferIndex;
            m_ColorAttachmentIndex = colorAttachment;
            m_DepthAttachmentIndex = depthAttachment;
        }
        else
        {
            GLuint framebuffers[2] = { 0, 0 }, resolveImageIndex = 0, renderbuffers[2] = { 0, 0 };
            glGenFramebuffers(2, framebuffers);
            glGenTextures(1, &resolveImageIndex);
            glGenRenderbuffers(2, renderbuffers);

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]);

            // Color attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[0]);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 
                static_cast<GLsizei>(samplesNumber), formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
            );
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffers[0]);

            // Depth attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[1]);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 
                static_cast<GLsizei>(samplesNumber), GL_DEPTH24_STENCIL8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
            );
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffers[1]);
            
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                JUMA_LOG(error, JSTR("Failed to initialize framebuffer"));
                glDeleteFramebuffers(2, framebuffers);
                glDeleteRenderbuffers(2, renderbuffers);
                glDeleteTextures(1, &resolveImageIndex);
                return false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]);

            // Color resolve attachment
            glBindTexture(GL_TEXTURE_2D, resolveImageIndex);
            glTexImage2D(GL_TEXTURE_2D, 
                0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                GL_RGB, GL_UNSIGNED_BYTE, nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveImageIndex, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                JUMA_LOG(error, JSTR("Failed to initialize framebuffer"));
                glDeleteFramebuffers(2, framebuffers);
                glDeleteRenderbuffers(2, renderbuffers);
                glDeleteTextures(1, &resolveImageIndex);
                return false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_FramebufferIndex = framebuffers[0];
            m_ColorAttachmentIndex = renderbuffers[0];
            m_DepthAttachmentIndex = renderbuffers[1];

            m_ResolveFramebufferIndex = framebuffers[1];
            m_ResolveColorAttachmentIndex = resolveImageIndex;
        }
        return true;
    }

    void RenderTarget_RenderAPIObject_OpenGL::clearData()
    {
        if (m_FramebufferIndex != 0)
        {
            glDeleteFramebuffers(1, &m_FramebufferIndex);
            if (!m_Parent->shouldResolveMultisampling())
            {
                glDeleteTextures(1, &m_ColorAttachmentIndex);
                glDeleteRenderbuffers(1, &m_DepthAttachmentIndex);
            }
            else
            {
                glDeleteRenderbuffers(1, &m_ColorAttachmentIndex);
                glDeleteRenderbuffers(1, &m_DepthAttachmentIndex);

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
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

#endif
