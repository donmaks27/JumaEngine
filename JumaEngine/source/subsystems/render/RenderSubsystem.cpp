// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

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
}
