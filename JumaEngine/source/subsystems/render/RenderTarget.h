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

    class RenderTarget_RenderAPIObject : public RenderAPIObject<RenderTarget>
    {
    public:
        RenderTarget_RenderAPIObject() = default;
        virtual ~RenderTarget_RenderAPIObject() override = default;
    };

    class RenderTarget final : public EngineContextObject, public RenderAPIWrapper<RenderTarget_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderTarget, EngineContextObject)

    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override;

        bool init(TextureFormat format, const math::uvector2& size);

        TextureFormat getFormat() const { return m_Format; }
        const math::uvector2& getSize() const { return m_Size; }

    protected:

        virtual RenderTarget_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        TextureFormat m_Format = TextureFormat::None;
        math::uvector2 m_Size = { 0, 0 };


        void clearData();
    };
}
