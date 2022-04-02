// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

#include "RenderSubsystem_Implementation.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
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

    void RenderSubsystem::clearSubsystemInternal()
    {
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
