// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "Material_OpenGL.h"
#include "RenderPipeline_OpenGL.h"
#include "RenderTarget_OpenGL.h"
#include "Shader_OpenGL.h"
#include "Texture_OpenGL.h"
#include "VertexBuffer_OpenGL.h"
#include "engine/Engine.h"
#include "subsystems/window/OpenGL/WindowSubsystem_OpenGL.h"

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
        m_WindowSubsystem = windowSubsystem->getRenderAPIObject<WindowSubsystem_RenderAPIObject_OpenGL>();

        if (windowSubsystem->createMainWindow(JSTR("JumaEngine"), { 800, 600 }) == INVALID_WINDOW_ID)
        {
            return false;
        }

        m_Parent->getRenderPipeline()->createRenderAPIObject();
        m_Parent->getDefaultTexture()->createRenderAPIObject();
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
        return createOpenGLObject<Shader_RenderAPIObject_OpenGL>();
    }
    Material_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createMaterialObject()
    {
        return createOpenGLObject<Material_RenderAPIObject_OpenGL>();
    }
    VertexBuffer_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createVertexBufferObject()
    {
        return createOpenGLObject<VertexBuffer_RenderAPIObject_OpenGL>();
    }
    Texture_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createTextureObject()
    {
        return createOpenGLObject<Texture_RenderAPIObject_OpenGL>();
    }
    RenderTarget_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createRenderTargetObject()
    {
        return createOpenGLObject<RenderTarget_RenderAPIObject_OpenGL>();
    }
    RenderPipeline_RenderAPIObject* RenderSubsystem_RenderAPIObject_OpenGL::createRenderPipelineObject()
    {
        return createOpenGLObject<RenderPipeline_RenderAPIObject_OpenGL>();
    }
}

#endif
