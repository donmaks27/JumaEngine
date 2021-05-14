// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Texture_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"

namespace JumaEngine
{
    Texture_OpenGL::~Texture_OpenGL()
    {
        terminate_OpenGL();
    }

    bool Texture_OpenGL::initTexture(const glm::uvec2& size, const TextureFormat format, const uint8* data)
    {
        if ((size.x == 0) || (size.y == 0) || (format == TextureFormat::None) || (data == nullptr))
        {
            return false;
        }

        GLenum glFormat = 0;
        const GLenum glType = GL_UNSIGNED_BYTE;
        switch (format)
        {
        case TextureFormat::R8G8B8: glFormat = GL_RGB; break;
        case TextureFormat::R8G8B8A8: glFormat = GL_RGBA; break;
        case TextureFormat::B8G8R8: glFormat = GL_BGR; break;
        case TextureFormat::B8G8R8A8: glFormat = GL_BGRA; break;
        case TextureFormat::R8: glFormat = GL_RED; break;
        case TextureFormat::None:
        default: ;
        }
        if (glFormat == 0)
        {
            return false;
        }

        m_Size = size;
        m_Format = format;

        glGenTextures(1, &m_TextureIndex);
        glBindTexture(GL_TEXTURE_2D, m_TextureIndex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, glFormat, glType, data);

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void Texture_OpenGL::terminate_OpenGL()
    {
        if (m_TextureIndex != 0)
        {
            glDeleteTextures(1, &m_TextureIndex);
            m_TextureIndex = 0;
            m_Size = { 0, 0 };
            m_Format = TextureFormat::None;
        }
    }
}

#endif
