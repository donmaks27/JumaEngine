// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem.h"

#include "engine/Engine.h"
#include "subsystems/render/RenderSubsystem.h"
#include "WindowSubsystem_Implementation.h"

namespace JumaEngine
{
    bool WindowSubsystem_RenderAPIObject::createWindowsFromParent()
    {
        if (m_Parent == nullptr)
        {
            return false;
        }

        for (const auto& parentDescription : m_Parent->getAllWindows())
        {
            if (!createWindow(parentDescription.key))
            {
                JUMA_LOG(error, JSTR("Failed to create window"));
                return false;
            }
        }
        return true;
    }

    WindowDescription* WindowSubsystem_RenderAPIObject::findParentWindow(const window_id_type windowID)
    {
        return m_Parent != nullptr ? m_Parent->m_Windows.find(windowID) : nullptr;
    }

    void WindowSubsystem_RenderAPIObject::onWindowResized(const window_id_type windowID, const math::uvector2& newSize)
    {
        WindowDescription* description = findParentWindow(windowID);
        if (description != nullptr)
        {
            description->size = newSize;
        }
    }

    WindowSubsystem_RenderAPIObject* WindowSubsystem::createRenderAPIObjectInternal()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Subsystem not initialized"));
            return nullptr;
        }
        return createWindowSubsystemRenderAPIObject(getOwnerEngine()->getRenderSubsystem()->getRenderAPI());
    }

    void WindowSubsystem::clearSubsystemInternal()
    {
        clearRenderAPIObject();
        m_Windows.clear();

        Super::clearSubsystemInternal();
    }

    window_id_type WindowSubsystem::createWindow(const jstring& title, const math::uvector2& size)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Subsystem not initialized"));
            return INVALID_WINDOW_ID;
        }

        const window_id_type windowID = m_WindowIDs.getUID();
        WindowDescription description;
        description.title = title;
        description.size = size;
        m_Windows.add(windowID, description);
        
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if ((renderObject != nullptr) && !renderObject->createWindow(windowID))
        {
            JUMA_LOG(error, JSTR("Failed to initialize window"));
            m_Windows.remove(windowID);
            return INVALID_WINDOW_ID;
        }
        return windowID;
    }
    void WindowSubsystem::destroyWindow(const window_id_type windowID)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Subsystem not initialized"));
            return;
        }
        if (!m_Windows.contains(windowID))
        {
            JUMA_LOG(info, JSTR("There is no window with ID ") + TO_JSTR(windowID));
            return;
        }

        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->destroyWindow(windowID);
        }
        m_Windows.remove(windowID);
    }
    bool WindowSubsystem::shouldCloseWindow(const window_id_type windowID) const
    {
        const RenderAPIObjectType* renderObject = getRenderAPIObject();
        return (renderObject != nullptr) && renderObject->shouldCloseWindow(windowID);
    }

    window_id_type WindowSubsystem::createMainWindow(const jstring& title, const math::uvector2& size)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Subsystem not initialized"));
            return INVALID_WINDOW_ID;
        }
        if (m_MainWindowID != INVALID_WINDOW_ID)
        {
            JUMA_LOG(warning, JSTR("Main window already created"));
            return m_MainWindowID;
        }

        const window_id_type windowID = createWindow(title, size);
        if (windowID == INVALID_WINDOW_ID)
        {
            JUMA_LOG(error, JSTR("Failed to create main window"));
            return INVALID_WINDOW_ID;
        }

        m_MainWindowID = windowID;
        return m_MainWindowID;
    }
    void WindowSubsystem::destroyMainWindow()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Subsystem not initialized"));
            return;
        }
        if (m_MainWindowID != INVALID_WINDOW_ID)
        {
            destroyWindow(m_MainWindowID);
        }
    }

    void WindowSubsystem::onStartRender()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->onStartRender();
        }
    }
    void WindowSubsystem::onStartWindowRender(const window_id_type windowID)
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->onStartWindowRender(windowID);
        }
    }
    void WindowSubsystem::onFinishWindowRender(const window_id_type windowID)
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->onFinishWindowRender(windowID);
        }
    }
    void WindowSubsystem::onFinishRender()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->onFinishRender();
        }
    }
}
