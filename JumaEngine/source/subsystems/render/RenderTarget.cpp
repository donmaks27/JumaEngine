// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

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

    bool RenderTarget::init(const TextureFormat format, const math::uvector2& size, const TextureSamples samples)
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
        m_TextureSamples = samples;
        markAsInitialized();
        return true;
    }
    bool RenderTarget::init(const window_id_type windowID, const TextureSamples samples)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render target already initiaized"));
            return false;
        }
        if (!getOwnerEngine()->getWindowSubsystem()->isWindowValid(windowID))
        {
            JUMA_LOG(error, JSTR("Invalid window ID"));
            return false;
        }

        m_WindowID = windowID;
        m_TextureSamples = samples;
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

        m_Format = TextureFormat::None;
        m_Size = { 0, 0 };
        m_TextureSamples = TextureSamples::SAMPLES_1;
        m_WindowID = INVALID_WINDOW_ID;
    }
}
