// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/texture/TextureBase.h>

namespace JumaEngine
{
    class TextureBase : public EngineContextObject
    {
        JUMAENGINE_CLASS_ABSTRACT(TextureBase, EngineContextObject)
            
    public:
        TextureBase() = default;
        virtual ~TextureBase() override = default;

        virtual JumaRE::TextureBase* getTextureBase() const { return nullptr; }

        math::uvector2 getSize() const
        {
	        const JumaRE::TextureBase* textureBase = getTextureBase();
            return textureBase != nullptr ? textureBase->getSize() : math::uvector2{ 0, 0 };
        }
    };
}
