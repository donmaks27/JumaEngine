// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderPipeline_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderOptions_OpenGL.h"
#include "RenderTarget_OpenGL.h"
#include "engine/Engine.h"
#include "subsystems/GlobalMaterialParamsSubsystem.h"
#include "subsystems/render/RenderTarget.h"
#include "subsystems/window/WindowSubsystem.h"
#include "subsystems/window/OpenGL/WindowSubsystem_OpenGL.h"

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
        WindowSubsystem* windowSubsystem = getWindowSubsystemObject()->getParent();

        windowSubsystem->onStartRender();

        RenderOptions_OpenGL options;
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

            options.renderTargetName = stageName;
            options.renderTarget = renderTarget;
            options.windowID = renderTargetObject->getUsingWindowID();
            callRenderForRenderTarget(renderTargetObject, options);
        }

        windowSubsystem->onFinishRender();

        return true;
    }
    bool RenderPipeline_RenderAPIObject_OpenGL::callRenderForRenderTarget(RenderTarget_RenderAPIObject_OpenGL* renderTargetObject, RenderOptions_OpenGL options)
    {
        getWindowSubsystemObject()->setWindowActive(renderTargetObject->getUsingWindowID());
        if (!renderTargetObject->startRender())
        {
            return false;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_CULL_FACE);
        math::matrix4 windowTransform(1);
        if (!renderTargetObject->getParent()->isWindowRenderTarget())
        {
            glCullFace(GL_BACK);
        }
        else
        {
            glCullFace(GL_FRONT);
            windowTransform[1][1] = -1;
        }
        getParent()->getOwnerEngine()->getGlobalMaterialParamsSubsystem()->getGlobalMaterialParams().setValue<ShaderUniformType::Mat4>(JSTR("WindowTransform"), windowTransform);
        
        renderPipelineStage(&options);

        renderTargetObject->finishRender();
        return true;
    }
}

#endif
