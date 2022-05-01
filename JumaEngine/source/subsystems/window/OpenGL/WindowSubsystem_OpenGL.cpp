// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

namespace JumaEngine
{
    void WindowSubsystem_RenderAPIObject_OpenGL::initOpenGL() const
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
        }
    }

    void WindowSubsystem_RenderAPIObject_OpenGL::setWindowActive(const window_id_type windowID)
    {
        if (m_ActiveWindow != windowID)
        {
            m_ActiveWindow = windowID;
            setWindowActiveInternal(windowID);
        }
    }
}

#endif
