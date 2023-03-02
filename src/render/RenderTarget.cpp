// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/render/RenderTarget.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    void RenderTarget::onObjectDescriptorDestroying()
    {
        destroyRenderTarget();
        Super::onObjectDescriptorDestroying();
    }

    void RenderTarget::destroyRenderTarget()
    {
        if (m_RenderTarget != nullptr)
        {
            getEngine()->getRenderEngine()->destroyRenderTarget(m_RenderTarget);
            m_RenderTarget = nullptr;
        }
    }
}
