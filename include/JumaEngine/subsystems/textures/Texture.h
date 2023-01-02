// Copyright © 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include <JumaRE/texture/Texture.h>
#include <jutils/jstringID.h>

namespace JumaEngine
{
    class TexturesSubsystem;

    class Texture final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Texture, EngineContextObject)

        friend TexturesSubsystem;

    public:
        Texture() = default;
        virtual ~Texture() override = default;

        JumaRE::Texture* getTexture() const { return m_Texture; }

    private:

        JumaRE::Texture* m_Texture = nullptr;


        bool loadTexture(const jstringID& textureName);
        void clearTexture();
    };
}
