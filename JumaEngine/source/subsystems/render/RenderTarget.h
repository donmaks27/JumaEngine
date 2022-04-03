// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "texture/TextureFormat.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class RenderTarget;

    class RenderTargetRenderAPIObject : public RenderAPIObject<RenderTarget>
    {
    public:
        RenderTargetRenderAPIObject() = default;
        virtual ~RenderTargetRenderAPIObject() override = default;
    };

    class RenderTarget final : public EngineContextObject, public RenderAPIWrapper<RenderTargetRenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderTarget, EngineContextObject)

    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override;

        bool init(TextureFormat format, const math::uvector2& size);

        TextureFormat getFormat() const { return m_Format; }
        math::uvector2 getSize() const { return m_Size; }

    protected:

        virtual RenderTargetRenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        TextureFormat m_Format = TextureFormat::None;
        math::uvector2 m_Size = { 0, 0 };


        void clearData();
    };
}
