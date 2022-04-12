// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderPipeline_OpenGL.h"

#include "subsystems/render/RenderOptions.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderTarget_OpenGL.h"
#include "subsystems/render/RenderTarget.h"

namespace JumaEngine
{
    RenderPipeline_RenderAPIObject_OpenGL::~RenderPipeline_RenderAPIObject_OpenGL()
    {
        clearData();
    }

    bool RenderPipeline_RenderAPIObject_OpenGL::initInternal()
    {
        return true;
    }

    void RenderPipeline_RenderAPIObject_OpenGL::clearData()
    {
    }

    bool RenderPipeline_RenderAPIObject_OpenGL::renderPipeline()
    {
        RenderOptions options;
        options.renderPipeline = m_Parent;
        for (const auto& stageName : m_Parent->getPipelineQueue())
        {
            const RenderPipelineStage* stage = m_Parent->getPipelineStage(stageName);
            const RenderTarget* renderTarget = stage != nullptr ? stage->renderTarget : nullptr;
            RenderTarget_RenderAPIObject_OpenGL* renderTargetObject = renderTarget != nullptr ? renderTarget->getRenderAPIObject<RenderTarget_RenderAPIObject_OpenGL>() : nullptr;
            if (renderTargetObject == nullptr)
            {
                continue;
            }

            renderTargetObject->startRender();

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            options.renderTargetName = stageName;
            renderPipelineStage(&options);

            if (renderTargetObject != nullptr)
            {
                renderTargetObject->finishRender();
            }
        }
        return true;
    }
}

#endif
