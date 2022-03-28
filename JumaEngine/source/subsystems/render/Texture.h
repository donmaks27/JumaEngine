// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureData;
    class Texture;

    class TextureRenderAPIObject : public RenderAPIObject<Texture>
    {
    public:
        TextureRenderAPIObject() = default;
        virtual ~TextureRenderAPIObject() override = default;

    protected:

        inline const TextureData* getTextureData() const;
    };

    class Texture : public EngineContextObject, public RenderObject<TextureRenderAPIObject>
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

        friend TextureRenderAPIObject;

    public:
        Texture() = default;
        virtual ~Texture() override;

        bool init(const TextureData* data);

    protected:

        virtual TextureRenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        const TextureData* m_Data = nullptr;


        void clearData();
    };

    const TextureData* TextureRenderAPIObject::getTextureData() const
    {
        return m_Parent != nullptr ? m_Parent->m_Data : nullptr;
    }
}
