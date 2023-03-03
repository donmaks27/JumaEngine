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
        for (const auto& renderTarget : m_RenderTargets)
        {
            RenderTarget* renderTargetPtr = renderTarget.get();
            if (renderTargetPtr != nullptr)
            {
                renderTargetPtr->destroyRenderTarget();
            }
        }
        m_RenderTargets.clear();

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
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        WidgetsCreator* widgetsCreator = getEngine()->getWidgetsCreator();

		JumaRE::RenderTarget* windowRenderTargetPtr = renderEngine->getRenderTarget(windowData->windowRenderTargetID);
        JumaRE::RenderTarget* renderTargetPtr = renderEngine->createRenderTarget(windowRenderTargetPtr->getColorFormat(), windowRenderTargetPtr->getSize(), windowRenderTargetPtr->getSampleCount());
        EngineObjectPtr<RenderTarget> windowRenderTarget = createRenderTarget(windowRenderTargetPtr);
        EngineObjectPtr<RenderTarget> renderTarget = createRenderTarget(renderTargetPtr);
        windowRenderTargetPtr->setSampleCount(JumaRE::TextureSamples::X1);
        renderEngine->getRenderPipeline()->addRenderTargetDependecy(windowData->windowRenderTargetID, renderTargetPtr->getID());
        renderTargetPtr->setupRenderStages({ { false } });

        const EngineObjectPtr<ImageWidget> imageWidget = widgetsCreator->createWidget<ImageWidget>();
        imageWidget->setUsingSolidColor(false);
        imageWidget->setTexture(renderTarget);
        if (renderEngine->getRenderAPI() == JumaRE::RenderAPI::OpenGL)
        {
            imageWidget->setTextureScale({ 1.0f, -1.0f });
        }

        const EngineObjectPtr<CursorWidget> cursorWidget = widgetsCreator->createWidget<CursorWidget>();

        const EngineObjectPtr<OverlayWidget> overlayWidget = widgetsCreator->createWidget<OverlayWidget>();
        overlayWidget->addWidget(imageWidget);
        overlayWidget->addWidget(cursorWidget);

        EngineObjectPtr<WidgetContext> widgetContext = widgetsCreator->createWidgetContext({ windowRenderTarget, 0 });
        widgetContext->setRootWidget(overlayWidget);

        m_WindowProxyRenderTargets.add(windowData->windowRenderTargetID, { std::move(windowRenderTarget), std::move(renderTarget), std::move(widgetContext) });
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

        for (const auto& proxyRenderTarget : m_WindowProxyRenderTargets)
        {
            destroyProxyWindowRenderTarget(proxyRenderTarget.key);
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
        renderTarget->renderTarget->destroyRenderTarget();
        renderTarget->renderTarget = nullptr;
        renderTarget->proxyRenderTarget->destroyRenderTarget();
        renderTarget->proxyRenderTarget = nullptr;
        return true;
	}

    EngineObjectPtr<RenderTarget> RenderEngineSubsystem::createRenderTarget(JumaRE::RenderTarget* renderTarget)
    {
        if (renderTarget == nullptr)
        {
            return nullptr;
        }

        EngineObjectPtr<RenderTarget> createdRenderTarget = getEngine()->createObject<RenderTarget>();
        if (createdRenderTarget == nullptr)
        {
            return nullptr;
        }

        createdRenderTarget->m_RenderTarget = renderTarget;
        m_RenderTargets.add(createdRenderTarget);
        return createdRenderTarget;
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
