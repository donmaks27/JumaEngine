// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

#include "engine/Engine.h"

namespace JumaEngine
{
    void RenderSubsystem::createMainWindow()
    {
        if (m_MainWindow == nullptr)
        {
            m_MainWindow = createWindowInternal({ 800, 600 }, JSTR("JumaEngine"));
        }
    }
    void RenderSubsystem::terminateMainWindow()
    {
        if (m_MainWindow != nullptr)
        {
            terminateWindowInternal(m_MainWindow);
            delete m_MainWindow;
            m_MainWindow = nullptr;
        }
    }

    void RenderSubsystem::setMainWindowSize(const glm::uvec2& size)
    {
        if (m_MainWindow != nullptr)
        {
            setWindowSizeInternal(m_MainWindow, size);
            m_MainWindow->size = size;
        }
    }
    void RenderSubsystem::setMainWindowTitle(const jstring& title)
    {
        if (m_MainWindow != nullptr)
        {
            setWindowTitleInternal(m_MainWindow, title);
            m_MainWindow->title = title;
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
