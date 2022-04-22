// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderPipeline_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderOptions_OpenGL.h"
#include "RenderTarget_OpenGL.h"
#include "engine/Engine.h"
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
        WindowSubsystem* windowSubsystem = getParent()->getOwnerEngine()->getWindowSubsystem();
        WindowSubsystem_RenderAPIObject_OpenGL* windowSubsystemObject = windowSubsystem->getRenderAPIObject<WindowSubsystem_RenderAPIObject_OpenGL>();

        jmap<jstringID, const ActionTaskResult<bool>*> windowRenderTaskResults;
        windowRenderTaskResults.reserve(m_Parent->getPipelineQueue().getSize());
        for (const auto& stageName : m_Parent->getPipelineQueue())
        {
            windowRenderTaskResults[stageName] = nullptr;
        }

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

            ActionTask renderTask;
            windowRenderTaskResults[stageName] = renderTask.bindClassMethod(
                this, &RenderPipeline_RenderAPIObject_OpenGL::callRenderForRenderTarget, 
                renderTargetObject, static_cast<const jmap<jstringID, const ActionTaskResult<bool>*>*>(&windowRenderTaskResults), options
            );
            if (!windowSubsystemObject->submitTaskForWindow(options.windowID, std::move(renderTask)))
            {
                windowRenderTaskResults[stageName] = nullptr;
            }
        }

        for (const auto& windowRenderTaskResult : windowRenderTaskResults)
        {
            if (windowRenderTaskResult.value != nullptr)
            {
                windowRenderTaskResult.value->waitForTaskFinished();
                windowRenderTaskResult.value->markAsHandled();
            }
        }
        windowSubsystem->onFinishRender();

        return true;
    }
    bool RenderPipeline_RenderAPIObject_OpenGL::callRenderForRenderTarget(RenderTarget_RenderAPIObject_OpenGL* renderTargetObject, 
        const jmap<jstringID, const ActionTaskResult<bool>*>* renderTaskResults, RenderOptions_OpenGL options)
    {
        const jset<jstringID>* dependencies = m_Parent->getPipelineStageDependencies(options.renderTargetName);
        if ((dependencies != nullptr) && !dependencies->isEmpty())
        {
            for (const auto& dependency : *dependencies)
            {
                const ActionTaskResult<bool>* const* renderTaskResultPtr = renderTaskResults->find(dependency);
                const ActionTaskResult<bool>* renderTaskResult = renderTaskResultPtr != nullptr ? *renderTaskResultPtr : nullptr;
                if (renderTaskResult == nullptr)
                {
                    return false;
                }

                renderTaskResult->waitForTaskFinished();
                if (!renderTaskResult->get())
                {
                    return false;
                }
            }
        }

        if (!renderTargetObject->startRender())
        {
            return false;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        renderPipelineStage(&options);

        renderTargetObject->finishRender();
        return true;
    }
}

#endif
