// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <jutils/jmap.h>

#include "Texture.h"

namespace JumaEngine
{
    class TexturesSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(TexturesSubsystem, EngineSubsystem)

    public:
        TexturesSubsystem() = default;
        virtual ~TexturesSubsystem() override = default;

        Texture* getTexture(const jstringID& textureName);

        void clear();

    protected:

        virtual bool initSubsystem() override;
        virtual void clearSubsystem() override;

    private:

        jmap<jstringID, Texture> m_Textures;


        void onRenderEngineDestroying(JumaRE::RenderEngine* renderEngine) { clearSubsystem(); }
    };
}
