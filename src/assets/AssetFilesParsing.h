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
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::json_value& outConfig, jstring& outErrorMessage);
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::json_value& outConfig);

    bool ParseTextureAssetFile(const jstring& assetPath, const json::json_value& config, TextureAssetCreateInfo& outCreateInfo, 
        jstring& outErrorMessage);
    bool ParseTextureAssetFile(const jstring& assetPath, const json::json_value& config, TextureAssetCreateInfo& outCreateInfo);

    bool ParseRenderTargetAssetFile(const jstring& assetPath, const json::json_value& config, RenderTargetCreateInfo& outCreateInfo);

    bool ParseMaterialAssetFile(const jstring& assetPath, const json::json_value& config, const JumaRE::RenderAPI renderAPI, 
        MaterialBaseCreateInfo& outCreateInfo, jstring& outErrorMessage);
    bool ParseMaterialAssetFile(const jstring& assetPath, const json::json_value& config, const JumaRE::RenderAPI renderAPI, 
        MaterialBaseCreateInfo& outCreateInfo);
    bool ParseMaterialInstanceAssetFile(const jstring& assetPath, const json::json_value& config, const EngineObjectPtr<Material>& parentMaterial, 
        MaterialInstanceCreateInfo& outCreateInfo);
}
