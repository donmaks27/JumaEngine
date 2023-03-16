// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/render/RenderTarget.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    bool RenderTarget::createAsset(const JumaRE::window_id windowID)
    {
        const JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        const JumaRE::WindowController* windowController = renderEngine != nullptr ? renderEngine->getWindowController() : nullptr;
        const JumaRE::WindowData* windowData = windowController != nullptr ? windowController->findWindowData(windowID) : nullptr;
        if (windowData == nullptr)
        {
            return false;
        }
        JumaRE::RenderTarget* renderTarget = renderEngine->getRenderTarget(windowData->windowRenderTargetID);
        if (renderTarget == nullptr)
        {
            return false;
        }
        m_RenderTarget = renderTarget;
        return true;
    }
    bool RenderTarget::createAsset(const RenderTargetCreateInfo &createInfo)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        JumaRE::RenderTarget* renderTarget = renderEngine != nullptr ? renderEngine->createRenderTarget(createInfo.format, 
            createInfo.size, createInfo.samples) : nullptr;
        if (renderTarget == nullptr)
        {
            return false;
        }
        m_RenderTarget = renderTarget;
        return true;
    }

    void RenderTarget::clearAsset()
    {
        if (m_RenderTarget != nullptr)
        {
            Super::clearAsset();
            
            if (!m_RenderTarget->isWindowRenderTarget())
            {
	            getEngine()->getRenderEngine()->destroyRenderTarget(m_RenderTarget);
            }
            m_RenderTarget = nullptr;
        }
    }
}
