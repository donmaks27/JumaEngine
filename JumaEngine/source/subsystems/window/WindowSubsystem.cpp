// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem.h"

#include "engine/Engine.h"
#include "subsystems/render/RenderSubsystem.h"
#include "WindowSubsystem_Implementation.h"

namespace JumaEngine
{
    bool WindowSubsystemRenderAPIObject::createWindowsFromParent()
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

    WindowDescription* WindowSubsystemRenderAPIObject::findParentWindow(const window_id_type windowID)
    {
        return m_Parent != nullptr ? m_Parent->m_Windows.find(windowID) : nullptr;
    }

    void WindowSubsystemRenderAPIObject::onWindowResized(const window_id_type windowID, const math::uvector2& newSize)
    {
        WindowDescription* description = findParentWindow(windowID);
        if (description != nullptr)
        {
            description->size = newSize;
        }
    }

    WindowSubsystemRenderAPIObject* WindowSubsystem::createRenderAPIObjectInternal()
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
}
