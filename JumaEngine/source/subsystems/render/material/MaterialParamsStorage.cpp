// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "MaterialParamsStorage.h"

#include "subsystems/render/RenderTarget.h"
#include "subsystems/render/Texture.h"

namespace JumaEngine
{
    MaterialParamsStorage::~MaterialParamsStorage()
    {
        clear();
    }

    void MaterialParamsStorage::setValueInternal_Texture(const jstringID& name, Texture* value)
    {
        Texture*& valueRef = m_UniformValues_Texture[name];

        Texture* oldValue = (valueRef != nullptr) && valueRef->isValid() ? valueRef : nullptr;
        Texture* newValue = (value != nullptr) && value->isValid() ? value : nullptr;
        if (oldValue != newValue)
        {
            if (oldValue != nullptr)
            {
                oldValue->onClear.unbind(this, &MaterialParamsStorage::onTextureCleared);
            }
            if (newValue != nullptr)
            {
                newValue->onClear.bind(this, &MaterialParamsStorage::onTextureCleared);
            }
            valueRef = newValue;
        }
    }
    void MaterialParamsStorage::onTextureCleared(Texture* texture)
    {
        for (auto& textureValue : m_UniformValues_Texture)
        {
            if (textureValue.value == texture)
            {
                textureValue.value = nullptr;
            }
        }
    }

    void MaterialParamsStorage::setValueInternal_RenderTarget(const jstringID& name, RenderTarget* value)
    {
        RenderTarget*& valueRef = m_UniformValues_RenderTarget[name];

        RenderTarget* oldValue = (valueRef != nullptr) && valueRef->isValid() ? valueRef : nullptr;
        RenderTarget* newValue = (value != nullptr) && value->isValid() ? value : nullptr;
        if (oldValue != newValue)
        {
            if (oldValue != nullptr)
            {
                oldValue->onClear.unbind(this, &MaterialParamsStorage::onRenderTargetCleared);
            }
            if (newValue != nullptr)
            {
                newValue->onClear.bind(this, &MaterialParamsStorage::onRenderTargetCleared);
            }
            valueRef = newValue;
        }
    }
    void MaterialParamsStorage::onRenderTargetCleared(RenderTarget* renderTarget)
    {
        for (auto& renderTargetValue : m_UniformValues_RenderTarget)
        {
            if (renderTargetValue.value == renderTarget)
            {
                renderTargetValue.value = nullptr;
            }
        }
    }

    bool MaterialParamsStorage::setDefaultValue(const ShaderUniformType type, const jstringID& name)
    {
        if (name != jstringID_NONE)
        {
            switch (type)
            {
            case ShaderUniformType::Mat4: return setValue<ShaderUniformType::Mat4>(name, math::matrix4(1));
            case ShaderUniformType::Texture: return setValue<ShaderUniformType::Texture>(name, nullptr);
            case ShaderUniformType::RenderTarget: return setValue<ShaderUniformType::RenderTarget>(name, nullptr);
            default: break;
            }
        }
        return false;
    }
    void MaterialParamsStorage::removeValue(const ShaderUniformType type, const jstringID& name)
    {
        switch (type)
        {
        case ShaderUniformType::Mat4: m_UniformValues_Mat4.remove(name); break;
        case ShaderUniformType::Texture: m_UniformValues_Texture.remove(name); break;
        case ShaderUniformType::RenderTarget: m_UniformValues_RenderTarget.remove(name); break;
        default: ;
        }
    }

    bool MaterialParamsStorage::contains(const ShaderUniformType type, const jstringID& name) const
    {
        switch (type)
        {
        case ShaderUniformType::Mat4: return m_UniformValues_Mat4.contains(name);
        case ShaderUniformType::Texture: return m_UniformValues_Texture.contains(name);
        case ShaderUniformType::RenderTarget: return m_UniformValues_RenderTarget.contains(name);
        default: ;
        }
        return false;
    }

    void MaterialParamsStorage::clear()
    {
        for (const auto& texture : m_UniformValues_Texture)
        {
            if (texture.value != nullptr)
            {
                texture.value->onClear.unbind(this, &MaterialParamsStorage::onTextureCleared);
            }
        }
        for (const auto& renderTarget : m_UniformValues_RenderTarget)
        {
            if (renderTarget.value != nullptr)
            {
                renderTarget.value->onClear.unbind(this, &MaterialParamsStorage::onRenderTargetCleared);
            }
        }

        m_UniformValues_Mat4.clear();
        m_UniformValues_Texture.clear();
        m_UniformValues_RenderTarget.clear();
    }
}
