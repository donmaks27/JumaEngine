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

        GLuint framebufferIndex = 0;
        GLuint colorAttachment = 0;
        GLuint depthAttachment = 0;
        glGenFramebuffers(1, &framebufferIndex);
        glGenTextures(1, &colorAttachment);
        glGenRenderbuffers(1, &depthAttachment);
        if ((framebufferIndex == 0) || (colorAttachment == 0) || (depthAttachment == 0))
        {
            JUMA_LOG(error, JSTR("Failed to generate framebuffer"));
            if (framebufferIndex != 0)
            {
                glDeleteFramebuffers(1, &framebufferIndex);
            }
            if (colorAttachment != 0)
            {
                glDeleteTextures(1, &colorAttachment);
            }
            if (depthAttachment != 0)
            {
                glDeleteRenderbuffers(1, &depthAttachment);
            }
            return false;
        }

        const math::uvector2& size = m_Parent->getSize();
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndex);

        glBindTexture(GL_TEXTURE_2D, colorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 
            0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
            GL_RGB, GL_UNSIGNED_BYTE, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
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
        m_ColorImageIndex = colorAttachment;
        m_DepthRenderbufferIndex = depthAttachment;
        return true;
    }

    void RenderTarget_RenderAPIObject_OpenGL::clearData()
    {
        if (m_FramebufferIndex != 0)
        {
            glDeleteFramebuffers(1, &m_FramebufferIndex);
            glDeleteTextures(1, &m_ColorImageIndex);
            glDeleteRenderbuffers(1, &m_DepthRenderbufferIndex);

            m_FramebufferIndex = 0;
        }
    }

    void RenderTarget_RenderAPIObject_OpenGL::startRender()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferIndex);
    }
    void RenderTarget_RenderAPIObject_OpenGL::finishRender()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

#endif
