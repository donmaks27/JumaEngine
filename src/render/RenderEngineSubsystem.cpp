// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/render/RenderEngineSubsystem.h"

#include <JumaRE/RenderPipeline.h>

#include "JumaEngine/Engine.h"
#include "JumaEngine/widgets/CursorWidget.h"
#include "JumaEngine/widgets/ImageWidget.h"
#include "JumaEngine/widgets/OverlayWidget.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

namespace JumaEngine
{
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
	void RenderEngineSubsystem::destroyProxyWindowRenderTargets()
	{
		JumaRE::WindowController* windowController = getEngine()->getRenderEngine()->getWindowController();
        windowController->onWindowCreated.unbind(this, &RenderEngineSubsystem::onWindowCreated);
        windowController->onWindowDestroying.unbind(this, &RenderEngineSubsystem::onWindowDestroying);

        for (const auto& proxyRenderTarget : m_WindowProxyRenderTargets)
        {
            destroyProxyWindowRenderTarget(proxyRenderTarget.key);
        }
	}
    bool RenderEngineSubsystem::destroyProxyWindowRenderTarget(const JumaRE::window_id windowID)
	{
        const WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowID);
        if (renderTarget == nullptr)
        {
	        return false;
        }
        
        WidgetsCreator* widgetsCreator = getEngine()->getWidgetsCreator();
        Widget* widget = renderTarget->widgetContext->getRootWidget();
        widgetsCreator->destroyWidgetContext(renderTarget->widgetContext);
        widgetsCreator->destroyWidget(widget, true);
        getEngine()->getRenderEngine()->destroyRenderTarget(renderTarget->proxyRenderTarget);
        return true;
	}

	void RenderEngineSubsystem::onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
	{
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        WidgetsCreator* widgetsCreator = getEngine()->getWidgetsCreator();

		JumaRE::RenderTarget* windowRenderTarget = renderEngine->getRenderTarget(windowData->windowRenderTargetID);
        JumaRE::RenderTarget* renderTarget = renderEngine->createRenderTarget(windowRenderTarget->getColorFormat(), windowRenderTarget->getSize(), windowRenderTarget->getSampleCount());
        windowRenderTarget->setSampleCount(JumaRE::TextureSamples::X1);
        renderEngine->getRenderPipeline()->addRenderTargetDependecy(windowData->windowRenderTargetID, renderTarget->getID());
        renderTarget->setupRenderStages({ { false } });

        ImageWidget* imageWidget = widgetsCreator->createWidget<ImageWidget>();
        imageWidget->setUsingSolidColor(false);
        imageWidget->setTexture(renderTarget);
        if (renderEngine->getRenderAPI() == JumaRE::RenderAPI::OpenGL)
        {
            imageWidget->setTextureScale({ 1.0f, -1.0f });
        }

        CursorWidget* cursorWidget = widgetsCreator->createWidget<CursorWidget>();

        OverlayWidget* overlayWidget = widgetsCreator->createWidget<OverlayWidget>();
        overlayWidget->addWidget(imageWidget);
        overlayWidget->addWidget(cursorWidget);

        WidgetContext* widgetContext = widgetsCreator->createWidgetContext({ windowRenderTarget, 0 });
        widgetsCreator->setRootWidget(widgetContext, overlayWidget);

        m_WindowProxyRenderTargets.add(windowData->windowRenderTargetID, { renderTarget, widgetContext });
	}
	void RenderEngineSubsystem::onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
	{
        if (destroyProxyWindowRenderTarget(windowData->windowID))
        {
	        m_WindowProxyRenderTargets.remove(windowData->windowID);
        }
	}

    JumaRE::RenderTarget* RenderEngineSubsystem::getWindowRenderTarget(const JumaRE::window_id windowID) const
	{
        const WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowID);
        if (renderTarget != nullptr)
        {
	        return renderTarget->proxyRenderTarget;
        }

        const JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        const JumaRE::WindowData* windowData = renderEngine != nullptr ? renderEngine->getWindowController()->findWindowData(windowID) : nullptr;
        return windowData != nullptr ? renderEngine->getRenderTarget(windowData->windowRenderTargetID) : nullptr;
	}
}
