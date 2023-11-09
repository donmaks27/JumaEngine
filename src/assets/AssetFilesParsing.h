// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "JumaEngine/core.h"

#include <JumaRE/RenderAPI.h>
#include <jutils/configs/json.h>

#include "JumaEngine/assets/AssetType.h"
#include "JumaEngine/assets/MaterialBase.h"
#include "JumaEngine/assets/MaterialInstance.h"
#include "JumaEngine/assets/Texture.h"
#include "JumaEngine/render/RenderTarget.h"

namespace JumaEngine
{
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::value& outConfig, jstring& outErrorMessage);

    bool ParseTextureAssetFile(const jstring& assetPath, const json::value& config, TextureAssetCreateInfo& outCreateInfo,
        jstring& outErrorMessage);

    bool ParseRenderTargetAssetFile(const jstring& assetPath, const json::value& config, RenderTargetCreateInfo& outCreateInfo,
        jstring& outErrorMessage);

    bool ParseMaterialAssetFile(const jstring& assetPath, const json::value& config, const JumaRE::RenderAPI renderAPI,
        MaterialBaseCreateInfo& outCreateInfo, jstring& outErrorMessage);
    bool ParseMaterialInstanceAssetFile(const jstring& assetPath, const json::value& config, const EngineObjectPtr<Material>& parentMaterial,
        MaterialInstanceCreateInfo& outCreateInfo);
}
