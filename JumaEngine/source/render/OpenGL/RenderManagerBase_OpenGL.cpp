// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"
#include "Engine.h"
#include "utils/log.h"
#include "shader/Shader_OpenGL.h"
#include "vertexBuffer/VertexBuffer_OpenGL.h"
#include "renderTarget/RenderTargetDirect_OpenGL.h"

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
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Shader_OpenGL>() : nullptr;
    }
    VertexBufferBase* RenderManagerBase_OpenGL::createVertextBuffer()
    {
        return new VertexBuffer_OpenGL();
    }
    RenderTargetDirectBase* RenderManagerBase_OpenGL::createRenderTargetDirect()
    {
        Engine* engine = getOwnerEngine();
        if (engine != nullptr)
        {
            return engine->createObject<RenderTargetDirect_OpenGL>();
        }
        return nullptr;
    }
}

#endif
