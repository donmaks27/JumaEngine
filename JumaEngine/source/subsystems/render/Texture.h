// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/render/RenderObject.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureData;
    class Texture;

    class TextureObject : public RenderObject<Texture>
    {
    public:
        TextureObject() = default;
        virtual ~TextureObject() override = default;

    protected:

        inline const TextureData* getTextureData() const;
    };

    class Texture : public EngineContextObject
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

        friend TextureObject;

    public:
        Texture() = default;
        virtual ~Texture() override;

        bool init(const TextureData* data);
        bool isValid() const { return m_Initialized; }
        void clear();

        bool createRenderObject();
        TextureObject* getRenderObject() const { return m_RenderObject; }
        void clearRenderObject();

    private:

        bool m_Initialized = false;

        const TextureData* m_Data = nullptr;

        TextureObject* m_RenderObject = nullptr;
    };

    const TextureData* TextureObject::getTextureData() const
    {
        return m_Parent != nullptr ? m_Parent->m_Data : nullptr;
    }
}
