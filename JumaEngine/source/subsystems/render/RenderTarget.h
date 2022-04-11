﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "texture/TextureFormat.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"
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


        bool init(TextureFormat format, const math::uvector2& size);

        TextureFormat getFormat() const { return m_Format; }
        const math::uvector2& getSize() const { return m_Size; }

        TextureSamples getTextureSamples() const { return m_TextureSamples; }
        bool shouldResolveMultisampling() const { return m_TextureSamples != TextureSamples::SAMPLES_1; }
        
    protected:

        virtual RenderTarget_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override;

    private:

        TextureFormat m_Format = TextureFormat::None;
        math::uvector2 m_Size = { 0, 0 };
        TextureSamples m_TextureSamples = TextureSamples::SAMPLES_4;


        void clearData();
    };
}
