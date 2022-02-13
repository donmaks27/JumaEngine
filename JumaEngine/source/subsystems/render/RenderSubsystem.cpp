// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    bool RenderSubsystem::createMainWindow()
    {
        if (m_MainWindow == nullptr)
        {
            m_MainWindow = getOwnerEngine()->getWindowSubsystem()->createWindow(JSTR("JumaEngine"), { 800, 600 });
        }
        return m_MainWindow != nullptr;
    }
    void RenderSubsystem::terminateMainWindow()
    {
        if (m_MainWindow != nullptr)
        {
            getOwnerEngine()->getWindowSubsystem()->destroyWindow(m_MainWindow->getID());
            m_MainWindow = nullptr;
        }
    }

    void RenderSubsystem::callEngineRender(const RenderOptions& options) const
    {
        Engine* engine = getOwnerEngine();
        if (engine != nullptr)
        {
            engine->render(options);
        }
    }
}
