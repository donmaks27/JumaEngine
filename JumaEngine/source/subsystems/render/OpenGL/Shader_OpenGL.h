// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Shader.h"

namespace JumaEngine
{
    class Shader_OpenGL : public Shader
    {
        JUMAENGINE_CLASS(Shader_OpenGL, Shader)

    public:
        Shader_OpenGL() = default;
        virtual ~Shader_OpenGL() override;

        uint32 getProgramIndex() const { return isValid() ? m_ShaderProgramIndex : 0; }

        void activate() const;
        static void deactivate();

    protected:

        virtual bool initInternal(const jstring& shaderName, const jarray<ShaderUniform>& uniforms) override;
        virtual void clearInternal() override;

    private:

        uint32 m_ShaderProgramIndex = 0;


        bool loadAndCompileShader(const jstring& shaderFilePath, ShaderStage shaderType, uint32& outShaderIndex) const;
        bool loadShaderText(const jstring& shaderFilePath, jarray<jstring>& outShaderText) const;
        bool compileShader(const jarray<jstring>& shaderText, ShaderStage shaderType, uint32& outShaderIndex) const;
        bool compileShaderProgram(const jarray<uint32>& shaders, uint32& outShaderProgramIndex) const;

        void clearOpenGL();
    };
}

#endif
