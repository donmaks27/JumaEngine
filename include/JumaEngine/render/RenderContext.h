// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
	struct RenderContext
	{
		JumaRE::RenderTarget* renderTarget = nullptr;
		int32 renderStageIndex = 0;

		JumaRE::RenderTarget* getRenderTarget() const { return renderTarget; }
		bool addPrimitiveToRenderList(const JumaRE::RenderPrimitive& primitive) const
		{
			return (renderTarget != nullptr) && renderTarget->addPrimitiveToRenderStage(renderStageIndex, primitive);
		}

		constexpr bool operator<(const RenderContext& context) const
		{
			if (renderTarget < context.renderTarget)
			{
				return true;
			}
			return (renderTarget == context.renderTarget) && (renderStageIndex < context.renderStageIndex);
		}
	};
}
