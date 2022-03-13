// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "subsystems/render/RenderOptions.h"
#include "ShaderObject_OpenGL.h"
#include "MaterialObject_OpenGL.h"
#include "TextureObject_OpenGL.h"
#include "VertexBufferObject_OpenGL.h"

namespace JumaEngine
{
    bool RenderSubsystem_OpenGL::initSubsystemInternal()
    {
        if (!Super::initSubsystemInternal())
        {
            return false;
        }

        if (!createMainWindow())
        {
            return false;
        }

        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }
        return true;
    }

    void RenderSubsystem_OpenGL::clearSubsystemInternal()
    {
        terminateMainWindow();

        Super::clearSubsystemInternal();
    }

    void RenderSubsystem_OpenGL::render()
    {
        m_MainWindow->startRender();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        constexpr RenderOptions options;
        callEngineRender(&options);

        m_MainWindow->finishRender();
    }

    ShaderObject* RenderSubsystem_OpenGL::createShaderObject()
    {
        return new ShaderObject_OpenGL();
    }
    MaterialObject* RenderSubsystem_OpenGL::createMaterialObject()
    {
        return new MaterialObject_OpenGL();
    }
    VertexBufferObject* RenderSubsystem_OpenGL::createVertexBufferObject()
    {
        return new VertexBufferObject_OpenGL();
    }
    TextureObject* RenderSubsystem_OpenGL::createTextureObject()
    {
        return new TextureObject_OpenGL();
    }
}

#endif
