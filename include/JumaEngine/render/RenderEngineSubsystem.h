// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <JumaRE/RenderTarget.h>
#include <jutils/jmap.h>

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

		JumaRE::RenderTarget* getWindowRenderTarget(JumaRE::window_id windowID) const;
		
	private:

		struct WindowProxyRenderTarget
        {
	        JumaRE::RenderTarget* proxyRenderTarget = nullptr;
            EngineObjectPtr<WidgetContext> widgetContext = nullptr;
        };

		jmap<JumaRE::window_id, WindowProxyRenderTarget> m_WindowProxyRenderTargets;


		void createProxyWindowRenderTargets();
		void destroyProxyWindowRenderTargets();
		bool destroyProxyWindowRenderTarget(JumaRE::window_id windowID);
		void onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
        void onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
	};
}
