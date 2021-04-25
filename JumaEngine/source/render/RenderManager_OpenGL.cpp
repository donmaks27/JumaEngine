// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManager_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"
#include "utils/log.h"
#include "shader/ShaderOpenGL.h"
#include "vertexBuffer/VertexBufferOpenGL.h"

namespace JumaEngine
{
    bool RenderManager_OpenGL::initInternal()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }
        return true;
    }

    ShaderBase* RenderManager_OpenGL::createShader()
    {
        return new ShaderOpenGL();
    }
    VertexBufferBase* RenderManager_OpenGL::createVertextBuffer()
    {
        return new VertexBufferOpenGL();
    }

    void RenderManager_OpenGL::startRender()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

#endif
