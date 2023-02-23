// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/texture/Texture.h>
#include <jutils/jstringID.h>

namespace JumaEngine
{
    class AssetsEngineSubsystem;

    class Texture final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

        friend AssetsEngineSubsystem;

    public:
        Texture() = default;
        virtual ~Texture() override = default;

        JumaRE::Texture* getTexture() const { return m_Texture; }

    private:

        JumaRE::Texture* m_Texture = nullptr;

        
        bool loadTexture(const jstringID& textureName, const jstring& contentFolder);
        void clearTexture();
    };
}
