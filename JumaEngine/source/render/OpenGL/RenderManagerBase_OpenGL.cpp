// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "GL/glew.h"
#include "engine/Engine.h"
#include "utils/jlog.h"
#include "shader/Shader_OpenGL.h"
#include "vertexBuffer/VertexBuffer_OpenGL.h"
#include "renderTarget/RenderTargetDirect_OpenGL.h"
#include "texture/Texture_OpenGL.h"

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

    ShaderBase* RenderManagerBase_OpenGL::createShaderInternal()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Shader_OpenGL>() : nullptr;
    }
    VertexBufferBase* RenderManagerBase_OpenGL::createVertextBufferInternal()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<VertexBuffer_OpenGL>() : nullptr;
    }
    TextureBase* RenderManagerBase_OpenGL::createTextureInternal()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Texture_OpenGL>() : nullptr;
    }
    RenderTargetDirectBase* RenderManagerBase_OpenGL::createRenderTargetDirectInternal()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<RenderTargetDirect_OpenGL>() : nullptr;
    }
}

#endif
