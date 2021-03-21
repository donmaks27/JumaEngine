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

    float WindowBase::getWindowAspectRatio() const
    {
    	if (m_WindowSize.y > 0)
    	{
    		return static_cast<float>(m_WindowSize.x) / static_cast<float>(m_WindowSize.y);
    	}
    	return 0.0f;
    }
}
