// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "utils/jmap.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
    class Shader_OpenGL final : public ShaderBase
    {
        JUMAENGINE_CLASS(Shader_OpenGL, ShaderBase)

    public:
        Shader_OpenGL() = default;
        virtual ~Shader_OpenGL() override;

        enum class ShaderType
        {
            Vertex,
            Geometry,
            Fragment
        };

        virtual void cacheShaderUniformName(const char* uniformName) override;

    protected:

        virtual bool loadShaderInternal(const jstring& shaderName) override;
        virtual void clearShaderInternal() override;

        virtual void activateShaderInternal() override;
        virtual void deactivateShaderInternal() override { deactivateShaderOpenGL(); }

        virtual void setUniformValueInternal(const char* uniformName, bool value) override { setUniformValueInternal(uniformName, static_cast<int32>(value)); }
        virtual void setUniformValueInternal(const char* uniformName, int32 value) override;
        virtual void setUniformValueInternal(const char* uniformName, float value) override;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec2& value) override;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec3& value) override;
        virtual void setUniformValueInternal(const char* uniformName, const glm::vec4& value) override;
        virtual void setUniformValueInternal(const char* uniformName, const glm::mat4& value) override;
        virtual void setUniformValueInternal(const char* uniformName, const jarray<float>& value) override;

    private:

        uint32 m_ShaderProgramIndex = 0;
    	jmap<jstring, int32> m_CachedUniformLocations;


        bool loadShaderText(const jstring& shaderFilePath, jarray<jstring>& shaderText) const;
        uint32 compileShader(const jarray<jstring>& shaderText, ShaderType shaderType) const;
        uint32 loadAndCompileShader(const jstring& shaderFilePath, ShaderType shaderType) const;
        uint32 compileShaderProgram(uint32 vertexShader, uint32 geometryShader, uint32 fragmentShader) const;

        static void deactivateShaderOpenGL();
        void clearOpenGLShader();

        int32 getUniformLocation(const char* uniformName) const;
    };
}

#endif
