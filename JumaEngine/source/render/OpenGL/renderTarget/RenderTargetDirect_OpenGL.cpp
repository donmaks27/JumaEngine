// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderTargetDirect_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"

namespace JumaEngine
{
    void RenderTargetDirect_OpenGL::clearFrame()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

#endif
