// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    RenderTarget::~RenderTarget()
    {
        if (isValid())
        {
            onClear.call(this);
        }
        clearData();
    }

    RenderTarget_RenderAPIObject* RenderTarget::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createRenderTargetObject();
    }

    bool RenderTarget::init(const TextureFormat format, const math::uvector2& size)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render target already initiaized"));
            return false;
        }
        if ((format == TextureFormat::None) || (size.x == 0) || (size.y == 0))
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        m_Format = format;
        m_Size = size;
        markAsInitialized();
        return true;
    }

    void RenderTarget::clearInternal()
    {
        onClear.call(this);
        onClear.clear();

        clearData();
    }
    void RenderTarget::clearData()
    {
        clearRenderAPIObject();

        m_Size = { 0, 0 };
        m_Format = TextureFormat::None;
    }
}
