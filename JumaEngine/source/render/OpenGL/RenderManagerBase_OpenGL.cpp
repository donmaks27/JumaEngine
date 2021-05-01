// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"
#include "utils/log.h"
#include "shader/Shader_OpenGL.h"
#include "vertexBuffer/VertexBuffer_OpenGL.h"

namespace JumaEngine
{
    bool RenderManagerBase_OpenGL::initInternal()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }
        return true;
    }

    ShaderBase* RenderManagerBase_OpenGL::createShader()
    {
        return new Shader_OpenGL();
    }
    VertexBufferBase* RenderManagerBase_OpenGL::createVertextBuffer()
    {
        return new VertexBuffer_OpenGL();
    }

    void RenderManagerBase_OpenGL::startRender()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

#endif
