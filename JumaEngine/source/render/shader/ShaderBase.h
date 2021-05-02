// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/mat4x4.hpp"
#include "utils/jarray.h"

namespace JumaEngine
{
    class ShaderBase
    {
    public:
        ShaderBase() = default;
        virtual ~ShaderBase() = default;

        const jstring& getShaderName() const { return m_ShaderName; }

        bool loadShader(const jstring& shaderName);
        bool isShaderLoaded() const { return m_ShaderLoaded; }
        void clearShader();

        virtual void cacheShaderUniformName(const char* uniformName) {}

        void activate();
        bool isActive() const { return getActiveShader() == this; }
        void deactivate();

        static ShaderBase* getActiveShader() { return s_ActiveShader; }
        static bool hasActiveShader() { return getActiveShader() != nullptr; }
        static void deactivateActiveShader();

        template<typename T>
        static void setActiveShaderUniformValue(const char* uniformName, T value)
        {
            if (hasActiveShader())
            {
                s_ActiveShader->setUniformValue(uniformName, value);
            }
        }

    protected:

        virtual bool loadShaderInternal(const jstring& shaderName) = 0;
        virtual void clearShaderInternal() = 0;

        virtual void activateShaderInternal() = 0;
        virtual void deactivateShaderInternal() = 0;
        static void clearActiveShaderRef() { s_ActiveShader = nullptr; }

        virtual void setUniformValue(const char* uniformName, bool value) = 0;
        virtual void setUniformValue(const char* uniformName, int32 value) = 0;
        virtual void setUniformValue(const char* uniformName, float value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec2& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec3& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec4& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::mat4& value) = 0;
        virtual void setUniformValue(const char* uniformName, const jarray<float>& value) = 0;

    private:

        static ShaderBase* s_ActiveShader;

        bool m_ShaderLoaded = false;
        jstring m_ShaderName = jstring();
    };
}
