// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../assets/TextureBase.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class RenderEngineSubsystem;

    struct RenderTargetCreateInfo
    {
        JumaRE::TextureFormat format = JumaRE::TextureFormat::NONE; 
        math::uvector2 size = { 0, 0 }; 
        JumaRE::TextureSamples samples = JumaRE::TextureSamples::X1;
    };

    class RenderTarget final : public TextureBase
    {
        JUMAENGINE_CLASS(RenderTarget, TextureBase)

        friend AssetsEngineSubsystem;
        friend RenderEngineSubsystem;

    public:
        RenderTarget() : TextureBase(AssetType::RenderTarget) {}
        virtual ~RenderTarget() override = default;

        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderTarget; }
        virtual JumaRE::TextureBase* getTextureBase() const override { return getRenderTarget(); }

    protected:

        virtual void clearAsset() override;

    private:

        JumaRE::RenderTarget* m_RenderTarget = nullptr;


        bool createAsset(JumaRE::window_id windowID);
        bool createAsset(const RenderTargetCreateInfo& createInfo);
    };
}
