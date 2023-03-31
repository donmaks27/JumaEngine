// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <jutils/jmap.h>

#include "RenderTarget.h"

namespace JumaEngine
{
    class WidgetContext;

	class RenderEngineSubsystem final : public EngineSubsystem
	{
		JUMAENGINE_CLASS(RenderEngineSubsystem, EngineSubsystem)

		friend Engine;

	public:
		RenderEngineSubsystem() = default;
		virtual ~RenderEngineSubsystem() override = default;

		EngineObjectPtr<RenderTarget> createRenderTarget(const RenderTargetCreateInfo& createInfo);
		EngineObjectPtr<RenderTarget> getWindowRenderTarget(JumaRE::window_id windowID) const;

	protected:

		virtual void clearSubsystem() override;
		
	private:

		struct WindowProxyRenderTarget
        {
			EngineObjectPtr<RenderTarget> windowRenderTarget = nullptr;
	        EngineObjectPtr<RenderTarget> proxyRenderTarget = nullptr;
            EngineObjectPtr<WidgetContext> widgetContext = nullptr;
        };

		jmap<JumaRE::window_id, WindowProxyRenderTarget> m_WindowProxyRenderTargets;


		EngineObjectPtr<RenderTarget> createRenderTarget(JumaRE::window_id windowID);
		EngineObjectPtr<RenderTarget> createRenderTarget(JumaRE::RenderTarget* renderTarget);

		void createProxyWindowRenderTargets();
		void onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
        void onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
		void destroyProxyWindowRenderTargets();
		bool destroyProxyWindowRenderTarget(JumaRE::window_id windowID);
	};
}
