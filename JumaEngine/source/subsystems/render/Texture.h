// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"
#include "texture/TextureFormat.h"

namespace JumaEngine
{
    class TextureData;
    class Texture;

    class Texture_RenderAPIObject : public RenderAPIObject<Texture>
    {
    public:
        Texture_RenderAPIObject() = default;
        virtual ~Texture_RenderAPIObject() override = default;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnTextureEvent, Texture*, texture);

    class Texture : public EngineContextObject, public RenderAPIWrapper<Texture_RenderAPIObject>
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

    public:
        Texture() = default;
        virtual ~Texture() override;

        OnTextureEvent onClear;


        bool init(TextureFormat format, const math::uvector2& size, const uint8* data);

        const math::uvector2& getSize() const { return m_Size; }
        TextureFormat getFormat() const { return m_Format; }

        const uint8* getData() const { return m_Data; }

    protected:

        virtual Texture_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override;

    private:

        math::uvector2 m_Size = { 0, 0 };
        TextureFormat m_Format = TextureFormat::None;

        const uint8* m_Data = nullptr;


        void clearData();
    };
}
