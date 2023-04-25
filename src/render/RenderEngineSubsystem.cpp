// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/render/RenderEngineSubsystem.h"

#include <JumaRE/RenderPipeline.h>

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/render/RenderTarget.h"
#include "JumaEngine/widgets/CursorWidget.h"
#include "JumaEngine/widgets/ImageWidget.h"
#include "JumaEngine/widgets/OverlayWidget.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

namespace JumaEngine
{
    void RenderEngineSubsystem::clearSubsystem()
    {
        Super::clearSubsystem();
    }

	void RenderEngineSubsystem::createProxyWindowRenderTargets()
	{
		JumaRE::WindowController* windowController = getEngine()->getRenderEngine()->getWindowController();
        windowController->onWindowCreated.bind(this, &RenderEngineSubsystem::onWindowCreated);
        windowController->onWindowDestroying.bind(this, &RenderEngineSubsystem::onWindowDestroying);
        for (const auto& windowID : windowController->getWindowIDs())
        {
	        onWindowCreated(windowController, windowController->findWindowData(windowID));
        }
	}
	void RenderEngineSubsystem::onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
	{
        EngineObjectPtr<RenderTarget> windowRenderTarget = createRenderTarget(windowData->windowRenderTargetID);
        if (windowRenderTarget == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render target for window {}"), windowData->windowID);
            return;
        }
        JumaRE::RenderTarget* windowRenderTargetPtr = windowRenderTarget->getRenderTarget();
        EngineObjectPtr<RenderTarget> proxyRenderTarget = createRenderTarget({
            windowRenderTargetPtr->getColorFormat(), windowRenderTargetPtr->getSize(), windowRenderTargetPtr->getSampleCount()
        });
        if (proxyRenderTarget == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create proxy render target for window {}"), windowData->windowID);
            return;
        }
        JumaRE::RenderTarget* proxyRenderTargetPtr = proxyRenderTarget->getRenderTarget();

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        windowRenderTargetPtr->setSampleCount(JumaRE::TextureSamples::X1);
        windowRenderTarget->getRenderTarget()->setupRenderStages({ { false } });
        renderEngine->getRenderPipeline()->addRenderTargetDependecy(windowData->windowRenderTargetID, proxyRenderTargetPtr->getID());

        WidgetsCreator* widgetsCreator = getEngine()->getWidgetsCreator();

        const EngineObjectPtr<ImageWidget> imageWidget = widgetsCreator->createWidget<ImageWidget>();
        const EngineObjectPtr<OverlayWidget> overlayWidget = widgetsCreator->createWidget<OverlayWidget>();
        EngineObjectPtr<WidgetContext> widgetContext = widgetsCreator->createWidgetContext({ windowRenderTarget, 0 });
        if ((imageWidget == nullptr) || (overlayWidget == nullptr) || (widgetContext == nullptr))
        {
            JUTILS_LOG(error, JSTR("Failed to create widgets for window {}"), windowData->windowID);
            return;
        }

        imageWidget->setUsingSolidColor(false);
        imageWidget->setTexture(proxyRenderTarget);
        if (renderEngine->getRenderAPI() == JumaRE::RenderAPI::OpenGL)
        {
            imageWidget->setTextureScale({ 1.0f, -1.0f });
        }
        overlayWidget->addWidget(imageWidget);
        overlayWidget->addWidget(widgetsCreator->createWidget(CursorWidget::GetClassStatic()));

        widgetContext->setRootWidget(overlayWidget);

        m_WindowProxyRenderTargets.add(windowData->windowRenderTargetID, {
            std::move(windowRenderTarget), std::move(proxyRenderTarget), std::move(widgetContext)
        });
	}

	void RenderEngineSubsystem::onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
	{
        if (destroyProxyWindowRenderTarget(windowData->windowID))
        {
	        m_WindowProxyRenderTargets.remove(windowData->windowID);
        }
	}
	void RenderEngineSubsystem::destroyProxyWindowRenderTargets()
	{
		JumaRE::WindowController* windowController = getEngine()->getRenderEngine()->getWindowController();
        windowController->onWindowCreated.unbind(this, &RenderEngineSubsystem::onWindowCreated);
        windowController->onWindowDestroying.unbind(this, &RenderEngineSubsystem::onWindowDestroying);

        for (const auto& windowID : m_WindowProxyRenderTargets.keys())
        {
            destroyProxyWindowRenderTarget(windowID);
        }
        m_WindowProxyRenderTargets.clear();
	}
    bool RenderEngineSubsystem::destroyProxyWindowRenderTarget(const JumaRE::window_id windowID)
	{
        WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowID);
        if (renderTarget == nullptr)
        {
	        return false;
        }
        
        renderTarget->widgetContext->getRootWidget()->destroy(true);
        renderTarget->widgetContext->destroy();
        renderTarget->widgetContext = nullptr;
        renderTarget->windowRenderTarget->clearAsset();
        renderTarget->windowRenderTarget = nullptr;
        renderTarget->proxyRenderTarget->clearAsset();
        renderTarget->proxyRenderTarget = nullptr;
        return true;
	}

    EngineObjectPtr<RenderTarget> RenderEngineSubsystem::createRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        EngineObjectPtr<RenderTarget> renderTarget = getEngine()->createObject<RenderTarget>();
        if ((renderTarget == nullptr) || !renderTarget->createAsset(createInfo))
        {
            return nullptr;
        }
        return renderTarget;
    }
    EngineObjectPtr<RenderTarget> RenderEngineSubsystem::createRenderTarget(const JumaRE::window_id windowID)
    {
        EngineObjectPtr<RenderTarget> renderTarget = getEngine()->createObject<RenderTarget>();
        if ((renderTarget == nullptr) || !renderTarget->createAsset(windowID))
        {
            return nullptr;
        }
        return renderTarget;
    }
    EngineObjectPtr<RenderTarget> RenderEngineSubsystem::createRenderTarget(JumaRE::RenderTarget* renderTarget)
    {
        if (renderTarget == nullptr)
        {
            return nullptr;
        }

        EngineObjectPtr<RenderTarget> renderTargetPtr = getEngine()->createObject<RenderTarget>();
        if (renderTargetPtr == nullptr)
        {
            return nullptr;
        }

        renderTargetPtr->m_RenderTarget = renderTarget;
        return renderTargetPtr;
    }
    EngineObjectPtr<RenderTarget> RenderEngineSubsystem::getWindowRenderTarget(const JumaRE::window_id windowID) const
	{
        const WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowID);
        if (renderTarget != nullptr)
        {
	        return renderTarget->proxyRenderTarget;
        }
        return nullptr;
	}
}
