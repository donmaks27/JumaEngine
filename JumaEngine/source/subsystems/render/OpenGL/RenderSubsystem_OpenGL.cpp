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
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    RenderSubsystem_RenderAPIObject_OpenGL::~RenderSubsystem_RenderAPIObject_OpenGL()
    {
        clearOpenGL();
    }

    bool RenderSubsystem_RenderAPIObject_OpenGL::initInternal()
    {
        if (!Super::initInternal())
        {
            return false;
        }

        WindowSubsystem* windowSubsystem = m_Parent->getOwnerEngine()->getWindowSubsystem();
        if (windowSubsystem->createMainWindow(JSTR("JumaEngine"), { 800, 600 }) == INVALID_WINDOW_ID)
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

    void RenderSubsystem_RenderAPIObject_OpenGL::clearOpenGL()
    {
        clearData();

        WindowSubsystem* windowSubsystem = m_Parent->getOwnerEngine()->getWindowSubsystem();
        windowSubsystem->destroyMainWindow();
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
}

#endif
