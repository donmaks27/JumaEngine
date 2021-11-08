// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Image_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

namespace JumaEngine
{
    Image_OpenGL::~Image_OpenGL()
    {
        if (isValid())
        {
            clearOpenGL();
        }
    }

    uint32 Image_OpenGL::imageFormatToGLFormat(const ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::R8G8B8A8: return GL_RGBA;
        case ImageFormat::B8G8R8A8: return GL_BGRA;
        default: ;
        }
        return 0;
    }

    bool Image_OpenGL::initInternal(const glm::uvec2& size, const ImageFormat format, const uint8* data)
    {
        const GLenum glFormat = imageFormatToGLFormat(format);
        if (glFormat == 0)
        {
            return false;
        }

        glGenTextures(1, &m_Index);
        glBindTexture(GL_TEXTURE_2D, m_Index);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA, 
            size.x, size.y, 0, imageFormatToGLFormat(format), GL_UNSIGNED_BYTE, data
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

    void Image_OpenGL::clearInternal()
    {
        clearOpenGL();
    }
    void Image_OpenGL::clearOpenGL()
    {
        glDeleteTextures(1, &m_Index);
        m_Index = 0;
    }

    void Image_OpenGL::activate(const uint32 index) const
    {
        if (isValid())
        {
            glActiveTexture(GL_TEXTURE0 + index);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_Index);
        }
    }
    void Image_OpenGL::deactivate(const uint32 index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

#endif
