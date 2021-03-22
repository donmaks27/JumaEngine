// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/mat4x4.hpp"
#include <vector>

namespace JumaEngine
{
    class ShaderBase
    {
    public:
        ShaderBase() = default;
        virtual ~ShaderBase() = default;

        const std::string& getShaderName() const { return m_ShaderName; }

        bool loadShader(const std::string& shaderName);
        virtual bool isShaderLoaded() const = 0;
        void clearShader();

        void activateShader();
        bool isShaderActive() const { return getActiveShader() == this; }
        void deactivateShader();

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

        bool m_ShouldAlwaysDeactivateOldShader = true;


        virtual void loadShaderInternal(const std::string& shaderName) = 0;
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
        virtual void setUniformValue(const char* uniformName, const std::vector<float>& value) = 0;

    private:

        static ShaderBase* s_ActiveShader;

        std::string m_ShaderName = std::string();
    };
}
