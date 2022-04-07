﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

#include "RenderPipeline.h"
#include "RenderSubsystem_Implementation.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    void RenderSubsystem_RenderAPIObject::clearData()
    {
        RenderPipeline* renderPipeline = m_Parent->getRenderPipeline();
        if (renderPipeline != nullptr)
        {
            renderPipeline->clearRenderAPIObject();
        }
    }

    bool RenderSubsystem_RenderAPIObject::createMainWindow()
    {
        return m_Parent->createMainWindow();
    }
    void RenderSubsystem_RenderAPIObject::destroyMainWindow()
    {
        return m_Parent->destroyMainWindow();
    }

    RenderSubsystem_RenderAPIObject* RenderSubsystem::createRenderAPIObjectInternal()
    {
        return createRenderSubsystemRenderAPIObject(getRenderAPI());
    }
    
    bool RenderSubsystem::initSubsystemInternal()
    {
        if (!Super::initSubsystemInternal())
        {
            return false;
        }

        m_RenderPipeline = getOwnerEngine()->createObject<RenderPipeline>();
        m_RenderPipeline->init();
        return true;
    }
    void RenderSubsystem::clearSubsystemInternal()
    {
        delete m_RenderPipeline;
        m_RenderPipeline = nullptr;

        clearRenderAPIObject();
        destroyMainWindow();

        Super::clearSubsystemInternal();
    }

    bool RenderSubsystem::createMainWindow()
    {
        if (m_MainWindowID != INVALID_WINDOW_ID)
        {
            return true;
        }
        m_MainWindowID = getOwnerEngine()->getWindowSubsystem()->createWindow(JSTR("JumaEngine"), { 800, 600 });
        return m_MainWindowID != INVALID_WINDOW_ID;
    }
    void RenderSubsystem::destroyMainWindow()
    {
        if (m_MainWindowID != INVALID_WINDOW_ID)
        {
            getOwnerEngine()->getWindowSubsystem()->destroyWindow(m_MainWindowID);
            m_MainWindowID = INVALID_WINDOW_ID;
        }
    }
    bool RenderSubsystem::shouldCloseMainWindow() const
    {
        return getOwnerEngine()->getWindowSubsystem()->shouldCloseWindow(getMainWindowID());
    }

    ShaderRenderAPIObject* RenderSubsystem::createShaderObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createShaderObject() : nullptr;
    }
    MaterialRenderAPIObject* RenderSubsystem::createMaterialObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createMaterialObject() : nullptr;
    }
    VertexBufferRenderAPIObject* RenderSubsystem::createVertexBufferObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createVertexBufferObject() : nullptr;
    }
    TextureRenderAPIObject* RenderSubsystem::createTextureObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createTextureObject() : nullptr;
    }
    RenderTargetRenderAPIObject* RenderSubsystem::createRenderTargetObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createRenderTargetObject() : nullptr;
    }
    RenderPipelineRenderAPIObject* RenderSubsystem::createRenderPipelineObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createRenderPipelineObject() : nullptr;
    }

    void RenderSubsystem::render()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->render();
        }
    }
    void RenderSubsystem::waitForRenderFinish()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->waitForRenderFinish();
        }
    }
}
