// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "texture/TextureFormat.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"
#include "subsystems/window/WindowID.h"
#include "texture/TextureSamples.h"

namespace JumaEngine
{
    class RenderTarget;

    class RenderTarget_RenderAPIObject : public RenderAPIObject<RenderTarget>
    {
    public:
        RenderTarget_RenderAPIObject() = default;
        virtual ~RenderTarget_RenderAPIObject() override = default;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnRenderTargetEvent, RenderTarget*, renderTarget);

    class RenderTarget final : public EngineContextObject, public RenderAPIWrapper<RenderTarget_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderTarget, EngineContextObject)

    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override;

        OnRenderTargetEvent onClear;


        bool init(TextureFormat format, const math::uvector2& size, TextureSamples samples);
        bool init(window_id_type windowID, TextureSamples samples);

        window_id_type getWindowID() const { return m_WindowID; }
        bool isWindowRenderTarget() const { return getWindowID() != INVALID_WINDOW_ID; }

        TextureSamples getTextureSamples() const { return m_TextureSamples; }
        bool shouldResolveMultisampling() const { return m_TextureSamples != TextureSamples::X1; }

        TextureFormat getFormat() const { return m_Format; }
        math::uvector2 getSize() const;

    protected:

        virtual RenderTarget_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override;

    private:

        window_id_type m_WindowID = INVALID_WINDOW_ID;

        // Ignore it if window ID is valid
        math::uvector2 m_Size = { 0, 0 };
        TextureFormat m_Format = TextureFormat::None;

        TextureSamples m_TextureSamples = TextureSamples::X1;


        void clearData();
        void onWindowDestroyed(window_id_type windowID);
    };
}
