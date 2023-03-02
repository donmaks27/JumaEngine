// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../assets/TextureBase.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class RenderEngineSubsystem;

    class RenderTarget final : public TextureBase
    {
        JUMAENGINE_CLASS(RenderTarget, TextureBase)

        friend RenderEngineSubsystem;

    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override = default;

        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderTarget; }
        virtual JumaRE::TextureBase* getTextureBase() const override { return getRenderTarget(); }

    protected:

        virtual void onObjectDescriptorDestroying() override;

    private:

        JumaRE::RenderTarget* m_RenderTarget = nullptr;


        void destroyRenderTarget();
    };
}
