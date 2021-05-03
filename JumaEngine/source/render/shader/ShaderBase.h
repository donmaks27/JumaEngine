// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/mat4x4.hpp"
#include "render/window/window_id.h"
#include "utils/jarray.h"
#include "utils/jmap.h"

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

        void activate(window_id windowID);
        bool isActive(const window_id windowID) const { return getActiveShader(windowID) == this; }
        void deactivate(window_id windowID);

        static ShaderBase* getActiveShader(window_id windowID);
        static bool hasActiveShader(const window_id windowID) { return getActiveShader(windowID) != nullptr; }
        static void deactivateActiveShader(window_id windowID);

        template<typename T>
        static void setActiveShaderUniformValue(const window_id windowID, const char* uniformName, T value)
        {
            ShaderBase* shader = getActiveShader(windowID);
            if (shader != nullptr)
            {
                shader->setUniformValue(uniformName, value);
            }
        }

    protected:

        virtual bool loadShaderInternal(const jstring& shaderName) = 0;
        virtual void clearShaderInternal() = 0;

        virtual void activateShaderInternal() = 0;
        virtual void deactivateShaderInternal() = 0;
        static void clearActiveShaderRef(const window_id windowID) { s_ActiveShaders.remove(windowID); }

        virtual void setUniformValue(const char* uniformName, bool value) = 0;
        virtual void setUniformValue(const char* uniformName, int32 value) = 0;
        virtual void setUniformValue(const char* uniformName, float value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec2& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec3& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::vec4& value) = 0;
        virtual void setUniformValue(const char* uniformName, const glm::mat4& value) = 0;
        virtual void setUniformValue(const char* uniformName, const jarray<float>& value) = 0;

    private:

        static jmap<window_id, ShaderBase*> s_ActiveShaders;

        bool m_ShaderLoaded = false;
        jstring m_ShaderName = jstring();
    };
}
