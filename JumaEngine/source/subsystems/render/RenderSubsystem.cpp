// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    bool RenderSubsystem::createMainWindow()
    {
        if (m_MainWindow == nullptr)
        {
            m_MainWindow = createWindowInternal({ 800, 600 }, JSTR("JumaEngine"));
        }
        return m_MainWindow != nullptr;
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

    void RenderSubsystem::setWindowSize(WindowDescription* window, const math::uvector2& size)
    {
        if (window != nullptr)
        {
            setWindowSizeInternal(window, size);
            window->size = size;
            window->onSizeChanged.call(window);
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
