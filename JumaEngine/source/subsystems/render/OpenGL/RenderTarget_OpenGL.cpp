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
        const window_id_type prevActiveWindowID = getWindowSubsystemObject()->getActiveWindow();
        getWindowSubsystemObject()->setWindowActive(getUsingWindowID());
        const bool success = createFramebufferData();
        getWindowSubsystemObject()->setWindowActive(prevActiveWindowID);
        return success;
    }
    bool RenderTarget_RenderAPIObject_OpenGL::createFramebufferData()
    {
        const bool renderToWindow = m_Parent->isWindowRenderTarget();
        const bool shouldResolveMultisampling = m_Parent->shouldResolveMultisampling();
        if (!shouldResolveMultisampling && renderToWindow)
        {
            return true;
        }

        const TextureFormat format = !renderToWindow ? m_Parent->getFormat() : TextureFormat::RGBA_UINT8;
        const uint32 formatOpenGL = GetOpenGLFormatByTextureFormat(format);
        if (formatOpenGL == 0)
        {
            JUMA_LOG(error, JSTR("Unsupported render target format"));
            return false;
        }
        
        const bool depthEnabled = true;
        const uint8 samplesNumber = GetTextureSamplesNumber(m_Parent->getTextureSamples());
        const bool shouldCreateResolveFramebuffer = shouldResolveMultisampling && !renderToWindow;
        const math::uvector2& size = m_Parent->getSize();
        GLuint colorAttachment = 0, depthAttachment = 0, resolveAttachment = 0;
        GLuint framebufferIndices[2] = { 0, 0 };

        glGenFramebuffers(shouldCreateResolveFramebuffer != 0 ? 2 : 1, framebufferIndices);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[0]);
        if (shouldResolveMultisampling)
        {
            glGenRenderbuffers(1, &colorAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, colorAttachment);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                static_cast<GLsizei>(samplesNumber), formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)
            );
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorAttachment);
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
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
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
        if (shouldCreateResolveFramebuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferIndices[1]);
            glGenTextures(1, &resolveAttachment);
            glBindTexture(GL_TEXTURE_2D, resolveAttachment);
            glTexImage2D(GL_TEXTURE_2D, 
                0, formatOpenGL, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveAttachment, 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_FramebufferData = { colorAttachment, depthAttachment, resolveAttachment, framebufferIndices[0], framebufferIndices[1] };
        return true;
    }

    void RenderTarget_RenderAPIObject_OpenGL::clearData()
    {
        if (m_FramebufferData.framebuffer != 0)
        {
            const window_id_type prevActiveWindowID = getWindowSubsystemObject()->getActiveWindow();
            getWindowSubsystemObject()->setWindowActive(getUsingWindowID());
            glDeleteFramebuffers(1, &m_FramebufferData.framebuffer);
            if (m_FramebufferData.resolveFramebuffer != 0)
            {
                glDeleteFramebuffers(1, &m_FramebufferData.resolveFramebuffer);
            }
            getWindowSubsystemObject()->setWindowActive(prevActiveWindowID);

            if (m_Parent->shouldResolveMultisampling())
            {
                glDeleteRenderbuffers(1, &m_FramebufferData.colorAttachment);
            }
            else
            {
                glDeleteTextures(1, &m_FramebufferData.colorAttachment);
            }
            if (m_FramebufferData.depthAttachment != 0)
            {
                glDeleteRenderbuffers(1, &m_FramebufferData.depthAttachment);
            }
            if (m_FramebufferData.resolveColorAttachment != 0)
            {
                glDeleteTextures(1, &m_FramebufferData.resolveColorAttachment);
            }
        }
    }

    bool RenderTarget_RenderAPIObject_OpenGL::startRender()
    {
        if (m_Parent->isWindowRenderTarget())
        {
            getWindowSubsystemObject()->getParent()->onStartWindowRender(m_Parent->getWindowID());
        }
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferData.framebuffer);
        return true;
    }
    void RenderTarget_RenderAPIObject_OpenGL::finishRender()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (m_Parent->shouldResolveMultisampling())
        {
            const math::uvector2& size = m_Parent->getSize();

            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferData.framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferData.resolveFramebuffer);
            glBlitFramebuffer(
                0, 0, static_cast<GLint>(size.x), static_cast<GLint>(size.y), 
                0, 0, static_cast<GLint>(size.x), static_cast<GLint>(size.y), 
                GL_COLOR_BUFFER_BIT, GL_NEAREST
            );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        if (m_Parent->isWindowRenderTarget())
        {
            m_Parent->getOwnerEngine()->getWindowSubsystem()->onFinishWindowRender(m_Parent->getWindowID());
        }
    }
}

#endif
