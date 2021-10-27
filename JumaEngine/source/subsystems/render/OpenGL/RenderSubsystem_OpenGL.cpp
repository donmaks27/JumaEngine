// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "utils/jlog.h"
#include "engine/Engine.h"
#include "VertexBuffer_OpenGL.h"
#include "Shader_OpenGL.h"
#include "Material_OpenGL.h"
#include "Image_OpenGL.h"

namespace JumaEngine
{
    bool RenderSubsystem_OpenGL::initOpenGL()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }
        return true;
    }

    void RenderSubsystem_OpenGL::render(const RenderQuery& query)
    {
        RenderOptions options;
        options.query = query;
        options.invertFacesOrientation = false;

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        callEngineRender(options);
    }

    jshared_ptr<VertexBuffer> RenderSubsystem_OpenGL::createVertexBuffer()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<VertexBuffer_OpenGL>() : nullptr;
    }
    jshared_ptr<Shader> RenderSubsystem_OpenGL::createShader()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Shader_OpenGL>() : nullptr;
    }
    jshared_ptr<Material> RenderSubsystem_OpenGL::createMaterial()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Material_OpenGL>() : nullptr;
    }
    jshared_ptr<Image> RenderSubsystem_OpenGL::createImage()
    {
        Engine* engine = getOwnerEngine();
        return engine != nullptr ? engine->createObject<Image_OpenGL>() : nullptr;
    }
}

#endif
