// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureData;
    class Texture;

    class Texture_RenderAPIObject : public RenderAPIObject<Texture>
    {
    public:
        Texture_RenderAPIObject() = default;
        virtual ~Texture_RenderAPIObject() override = default;

    protected:

        inline const TextureData* getTextureData() const;
    };

    class Texture : public EngineContextObject, public RenderAPIWrapper<Texture_RenderAPIObject>
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

        friend Texture_RenderAPIObject;

    public:
        Texture() = default;
        virtual ~Texture() override;

        bool init(const TextureData* data);

    protected:

        virtual Texture_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        const TextureData* m_Data = nullptr;


        void clearData();
    };

    const TextureData* Texture_RenderAPIObject::getTextureData() const
    {
        return m_Parent != nullptr ? m_Parent->m_Data : nullptr;
    }
}
