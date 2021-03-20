// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "WindowBase.h"
#include "utils/math_functions.h"

namespace JumaEngine
{
    void WindowBase::setWindowSize(const glm::uvec2& windowSize)
    {
        if (m_WindowSize != windowSize)
        {
            m_WindowSize = windowSize;
            onWindowSizeChanged();
        }
    }
}
