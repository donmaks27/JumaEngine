﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderPipeline.h"

#include "RenderSubsystem.h"
#include "RenderTarget.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    void RenderPipeline_RenderAPIObject::renderPipelineStage(RenderOptions* options)
    {
        m_Parent->getOwnerEngine()->render(options);
    }

    RenderPipeline::~RenderPipeline()
    {
        clearData();
    }

    RenderPipeline_RenderAPIObject* RenderPipeline::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createRenderPipelineObject();
    }

    void RenderPipeline::clearData()
    {
        clearRenderAPIObject();

        m_PipelineStagesQueue.clear();
        m_PipelineStages.clear();
        m_PipelineQueueValid = false;
    }

    bool RenderPipeline::init()
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render pipeline already initialized"));
            return false;
        }

        m_PipelineQueueValid = true;
        markAsInitialized();
        return true;
    }

    bool RenderPipeline::addPipelineStage(const jstringID& name, RenderTarget* renderTarget)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render pipeline not initialized"));
            return false;
        }
        if ((name == jstringID_NONE) || (renderTarget == nullptr) || !renderTarget->isValid())
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        if (m_PipelineStages.contains(name))
        {
            JUMA_LOG(error, JSTR("Stage already exists"));
            return false;
        }

        RenderPipelineStage& newStage = m_PipelineStages.add(name);
        newStage.renderTarget = renderTarget;
        m_PipelineStagesQueue.add(name);

        m_PipelineQueueValid = false;
        return true;
    }
    bool RenderPipeline::removePipelineStage(const jstringID& name)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render pipeline not initialized"));
            return false;
        }

        const RenderPipelineStage* stage = m_PipelineStages.find(name);
        if (stage == nullptr)
        {
            return true;
        }
        for (auto& pipelineStage : m_PipelineStages)
        {
            if (pipelineStage.key != name)
            {
                pipelineStage.value.dependencies.remove(name);
            }
        }
        m_PipelineStagesQueue.remove(name);

        m_PipelineQueueValid = false;
        return true;
    }

    bool RenderPipeline::addPipelineStageDependency(const jstringID& name, const jstringID& dependencyName)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render pipeline not initialized"));
            return false;
        }
        if (dependencyName == jstringID_NONE)
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        RenderPipelineStage* stage = m_PipelineStages.find(name);
        if (stage == nullptr)
        {
            JUMA_LOG(error, JSTR("There is no stage ") + name.toString());
            return false;
        }
        if (stage->dependencies.contains(dependencyName))
        {
            return true;
        }

        const RenderPipelineStage* dependencyStage = m_PipelineStages.find(dependencyName);
        if (dependencyStage == nullptr)
        {
            JUMA_LOG(error, JSTR("There is no stage ") + dependencyName.toString());
            return false;
        }
        if (dependencyStage->renderTarget->isWindowRenderTarget())
        {
            JUMA_LOG(error, JSTR("You can't set window stage as dependency"));
            return false;
        }

        stage->dependencies.add(dependencyName);

        m_PipelineQueueValid = false;
        return true;
    }
    bool RenderPipeline::removePipelineStageDependency(const jstringID& name, const jstringID& dependencyName)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render pipeline not initialized"));
            return false;
        }

        RenderPipelineStage* stage = m_PipelineStages.find(name);
        if (stage == nullptr)
        {
            JUMA_LOG(error, JSTR("There is no stage ") + name.toString());
            return false;
        }

        if (stage->dependencies.remove(dependencyName))
        {
            m_PipelineQueueValid = false;
        }
        return true;
    }

    bool RenderPipeline::validatePipelineQueue()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render pipeline not initialized"));
            return false;
        }
        if (m_PipelineQueueValid)
        {
            return true;
        }

        m_PipelineStagesQueue.clear();
        jmap<jstringID, RenderPipelineStage> stagesCopy = m_PipelineStages;
        while (!stagesCopy.isEmpty())
        {
            jstringID nextStageName = jstringID_NONE;
            for (auto& stage : stagesCopy)
            {
                if (stage.value.dependencies.isEmpty())
                {
                    nextStageName = stage.key;
                    break;
                }
            }

            if (nextStageName == jstringID_NONE)
            {
                JUMA_LOG(error, JSTR("Failed to validate render pipeline stages queue"));
                return false;
            }

            m_PipelineStagesQueue.add(nextStageName);
            stagesCopy.remove(nextStageName);
            for (auto& stage : stagesCopy)
            {
                stage.value.dependencies.remove(nextStageName);
            }
        }
        m_PipelineQueueValid = true;

        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            if (!renderObject->onRenderPipelineUpdated())
            {
                JUMA_LOG(error, JSTR("Failed to update render pipeline API object"));
                m_PipelineQueueValid = false;
                return false;
            }
        }
        return true;
    }

    bool RenderPipeline::render()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject == nullptr)
        {
            return false;
        }
        return renderObject->renderPipeline();
    }
}
