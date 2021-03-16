// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerOpenGL.h"
#include <GL/glew.h>
#include "utils/log.h"

namespace JumaEngine
{
    RenderManagerOpenGL::RenderManagerOpenGL()
    {
    }
    RenderManagerOpenGL::~RenderManagerOpenGL()
    {
    }

    bool RenderManagerOpenGL::init()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }

        return true;
    }

    void RenderManagerOpenGL::startFrameRender()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}
