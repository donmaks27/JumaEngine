// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/AssetObject.h"
#include "glm/mat4x4.hpp"
#include "utils/jarray.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureBase;

    struct ShaderUniformTexture
    {
        const asset_ptr<TextureBase>* texture = nullptr;
        uint32 index = 0;
    };

    class ShaderBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(ShaderBase, EngineContextObject)

    public:
        ShaderBase() = default;
        virtual ~ShaderBase() override = default;

        const jstring& getShaderName() const { return m_ShaderName; }

        bool loadShader(const jstring& shaderName);
        bool isShaderLoaded() const { return m_ShaderLoaded; }
        void clearShader();

        virtual void cacheShaderUniformName(const char* uniformName) {}

        bool activate();
        void deactivate();

        template<typename T>
        void setUniformValue(const char* uniformName, T value) { setUniformValueInternal(uniformName, value); }

    protected:

        virtual bool loadShaderInternal(const jstring& shaderName) = 0;
        virtual void clearShaderInternal() = 0;

        virtual void activateShaderInternal() = 0;
        virtual void deactivateShaderInternal() = 0;

        virtual void setUniformValueInternal(const char* uniformName, bool value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, int32 value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, float value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec2& value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec3& value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec4& value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const glm::mat4& value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const jarray<float>& value) = 0;
        virtual void setUniformValueInternal(const char* uniformName, const ShaderUniformTexture& value) = 0;

    private:

        bool m_ShaderLoaded = false;
        jstring m_ShaderName = jstring();
    };
}
