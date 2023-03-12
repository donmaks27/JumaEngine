// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "TextureBase.h"

#include <JumaRE/texture/Texture.h>
#include <jutils/jstringID.h>

namespace JumaEngine
{
    struct TextureAssetCreateInfo
    {
	    jstring textureDataPath;
        JumaRE::TextureFormat textureFormat = JumaRE::TextureFormat::NONE;
    };

    class Texture final : public TextureBase
    {
        JUMAENGINE_CLASS(Texture, TextureBase)

        friend AssetsEngineSubsystem;

    public:
        Texture() : TextureBase(AssetType::Texture) {}
        virtual ~Texture() override = default;

        JumaRE::Texture* getTexture() const { return m_Texture; }
        virtual JumaRE::TextureBase* getTextureBase() const override { return getTexture(); }

    protected:

        virtual void clearAsset() override;

    private:

        JumaRE::Texture* m_Texture = nullptr;

        
        bool loadAsset(const TextureAssetCreateInfo& createInfo);
    };
}
