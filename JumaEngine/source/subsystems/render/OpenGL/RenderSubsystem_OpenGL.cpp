// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "Material_OpenGL.h"
#include "RenderPipeline_OpenGL.h"
#include "RenderTarget_OpenGL.h"
#include "Shader_OpenGL.h"
#include "Texture_OpenGL.h"
#include "VertexBuffer_OpenGL.h"

namespace JumaEngine
{
    RenderSubsystem_RenderAPIObject_OpenGL::~RenderSubsystem_RenderAPIObject_OpenGL()
    {
        clearData();
    }

    bool RenderSubsystem_RenderAPIObject_OpenGL::initInternal()
    {
        if (!Super::initInternal())
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

        m_Parent->getRenderPipeline()->createRenderAPIObject();
        return true;
    }

    void RenderSubsystem_RenderAPIObject_OpenGL::clearData()
    {
        destroyMainWindow();
    }

    Shader_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createShaderObject()
    {
        return new Shader_RenderAPIObject_OpenGL();
    }
    Material_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createMaterialObject()
    {
        return new Material_RenderAPIObject_OpenGL();
    }
    VertexBuffer_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createVertexBufferObject()
    {
        return new VertexBuffer_RenderAPIObject_OpenGL();
    }
    Texture_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createTextureObject()
    {
        return new Texture_RenderAPIObject_OpenGL();
    }
    RenderTarget_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createRenderTargetObject()
    {
        return new RenderTarget_RenderAPIObject_OpenGL();
    }
    RenderPipeline_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createRenderPipelineObject()
    {
        return new RenderPipeline_RenderAPIObject_OpenGL();
    }


    /*bool RenderSubsystem_OpenGL::initSubsystemInternal()
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
    }*/
}

#endif
