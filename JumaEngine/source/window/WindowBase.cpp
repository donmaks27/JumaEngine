// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "WindowBase.h"
#include "render/RenderManagerBase.h"
#include "utils/system_functions.h"
#include "utils/math_functions.h"
#include "utils/log.h"

namespace JumaEngine
{
	void WindowBase::setWindowTitle(const jstring& title)
	{
		if (m_WindowTitle != title)
		{
			m_WindowTitle = title;
			updateWindowTitle();
		}
	}

    void WindowBase::setWindowSize(const glm::uvec2& windowSize)
    {
        if (m_WindowSize != windowSize)
        {
            m_WindowSize = windowSize;
            updateWindowSize();
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

	bool WindowBase::init()
	{
		if (isInit())
        {
            JUMA_LOG(warning, JTEXT("Window already initialized."));
            return false;
        }

        const RenderManagerBase* renderManager = SystemFunctions::getRenderManager(this);
        if ((renderManager == nullptr) || !isSupportedRenderAPI(renderManager->getRenderAPI()))
        {
            JUMA_LOG(error, JTEXT("Unsupported render API!"));
            return false;
        }

		initInternal();
		return isInit();
	}
}
