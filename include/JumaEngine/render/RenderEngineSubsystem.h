// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <JumaRE/RenderTarget.h>
#include <jutils/jmap.h>

namespace JumaEngine
{
    class RenderTarget;
    class WidgetContext;

	class RenderEngineSubsystem final : public EngineSubsystem
	{
		JUMAENGINE_CLASS(RenderEngineSubsystem, EngineSubsystem)

		friend Engine;

	public:
		RenderEngineSubsystem() = default;
		virtual ~RenderEngineSubsystem() override = default;

		EngineObjectPtr<RenderTarget> createRenderTarget(JumaRE::RenderTarget* renderTarget);
		EngineObjectPtr<RenderTarget> getWindowRenderTarget(JumaRE::window_id windowID) const;

	protected:

		virtual void clearSubsystem() override;
		
	private:

		struct WindowProxyRenderTarget
        {
			EngineObjectPtr<RenderTarget> renderTarget = nullptr;
	        EngineObjectPtr<RenderTarget> proxyRenderTarget = nullptr;
            EngineObjectPtr<WidgetContext> widgetContext = nullptr;
        };

		jarray<EngineObjectWeakPtr<RenderTarget>> m_RenderTargets;
		jmap<JumaRE::window_id, WindowProxyRenderTarget> m_WindowProxyRenderTargets;


		void createProxyWindowRenderTargets();
		void onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
        void onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData);
		void destroyProxyWindowRenderTargets();
		bool destroyProxyWindowRenderTarget(JumaRE::window_id windowID);
	};
}
