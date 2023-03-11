// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Asset.h"

#include <JumaRE/texture/TextureBase.h>

namespace JumaEngine
{
    class TextureBase : public Asset
    {
        JUMAENGINE_CLASS_ABSTRACT(TextureBase, Asset)
            
    public:
        TextureBase() = default;
        virtual ~TextureBase() override = default;
    protected:
        TextureBase(const AssetType type) : Asset(type) {}
    public:

        virtual JumaRE::TextureBase* getTextureBase() const { return nullptr; }

        math::uvector2 getSize() const
        {
	        const JumaRE::TextureBase* textureBase = getTextureBase();
            return textureBase != nullptr ? textureBase->getSize() : math::uvector2{ 0, 0 };
        }
    };
}
