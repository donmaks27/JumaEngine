// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    bool RenderSubsystem::createMainWindow()
    {
        if (m_MainWindowID == INVALID_WINDOW_ID)
        {
            m_MainWindowID = getOwnerEngine()->getWindowSubsystem()->createWindow(JSTR("JumaEngine"), { 800, 600 });
        }
        return m_MainWindowID != INVALID_WINDOW_ID;
    }
    bool RenderSubsystem::shouldCloseMainWindow() const
    {
        return getOwnerEngine()->getWindowSubsystem()->shouldCloseWindow(m_MainWindowID);
    }
    math::uvector2 RenderSubsystem::getMainWindowSize() const
    {
        const WindowDescription* description = getOwnerEngine()->getWindowSubsystem()->findWindow(m_MainWindowID);
        return description != nullptr ? description->size : math::uvector2(0);
    }
    void RenderSubsystem::destroyMainWindow()
    {
        if (m_MainWindowID != INVALID_WINDOW_ID)
        {
            getOwnerEngine()->getWindowSubsystem()->destroyWindow(m_MainWindowID);
            m_MainWindowID = INVALID_WINDOW_ID;
        }
    }

    void RenderSubsystem::callEngineRender(const RenderOptions* options) const
    {
        Engine* engine = getOwnerEngine();
        if (engine != nullptr)
        {
            engine->render(options);
        }
    }
}
