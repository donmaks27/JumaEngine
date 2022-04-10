// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Texture_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

namespace JumaEngine
{
    TextureFormat GetTextureFormatByOpenGLFormat(const uint32 format)
    {
        switch (format)
        {
        case GL_RGBA: return TextureFormat::RGBA;
        case GL_BGRA: return TextureFormat::BGRA;
        default: ;
        }
        return TextureFormat::None;
    }
    uint32 GetOpenGLFormatByTextureFormat(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA: return GL_RGBA;
        case TextureFormat::BGRA: return GL_BGRA;
        default: ;
        }
        return 0;
    }

    Texture_RenderAPIObject_OpenGL::~Texture_RenderAPIObject_OpenGL()
    {
        clearOpenGL();
    }

    bool Texture_RenderAPIObject_OpenGL::initInternal()
    {
        const math::vector2& size = m_Parent->getSize();

        const GLenum format = GetOpenGLFormatByTextureFormat(m_Parent->getFormat());
        if (format == 0)
        {
            JUMA_LOG(error, JSTR("Unsupported texture format"));
            return false;
        }

        glGenTextures(1, &m_ImageIndex);
        glBindTexture(GL_TEXTURE_2D, m_ImageIndex);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, 
            format, GL_UNSIGNED_BYTE, m_Parent->getData()
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    void Texture_RenderAPIObject_OpenGL::clearOpenGL()
    {
        if (m_ImageIndex != 0)
        {
            glDeleteTextures(1, &m_ImageIndex);
            m_ImageIndex = 0;
        }
    }

    bool Texture_RenderAPIObject_OpenGL::bind(const uint32 imageIndex, const uint32 index)
    {
        if (imageIndex == 0)
        {
            return false;
        }

        glActiveTexture(GL_TEXTURE0 + index);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, imageIndex);
        return true;
    }
    void Texture_RenderAPIObject_OpenGL::unbind(const uint32 index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

#endif
