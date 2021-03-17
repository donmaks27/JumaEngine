// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "ShaderBase.h"
#include <vector>

namespace JumaEngine
{
    class ShaderOpenGL : public ShaderBase
    {
    public:
        ShaderOpenGL();
        virtual ~ShaderOpenGL() override;

        enum class ShaderType
        {
            Vertex,
            Geometry,
            Fragment
        };

        virtual bool isShaderLoaded() const override { return m_ShaderProgramIndex != 0; }

    protected:

        virtual void loadShaderInternal(const std::string& shaderName) override;
        virtual void clearShaderInternal() override;

        virtual void activateShaderInternal() override;
        virtual void deactivateShaderInternal() override;

        virtual void setUniformValue(const char* uniformName, bool value) override { setUniformValue(uniformName, static_cast<int32>(value)); }
        virtual void setUniformValue(const char* uniformName, int32 value) override;
        virtual void setUniformValue(const char* uniformName, float value) override;
        virtual void setUniformValue(const char* uniformName, const glm::vec2& value) override;
        virtual void setUniformValue(const char* uniformName, const glm::vec3& value) override;
        virtual void setUniformValue(const char* uniformName, const glm::vec4& value) override;
        virtual void setUniformValue(const char* uniformName, const glm::mat4& value) override;
        virtual void setUniformValue(const char* uniformName, const std::vector<float>& value) override;

    private:

        uint32 m_ShaderProgramIndex = 0;


        bool loadShaderText(const std::string& shaderFilePath, std::vector<std::string>& shaderText) const;
        uint32 compileShader(const std::vector<std::string>& shaderText, ShaderType shaderType) const;
        uint32 loadAndCompileShader(const std::string& shaderFilePath, ShaderType shaderType) const;
        uint32 compileShaderProgram(uint32 vertexShader, uint32 geometryShader, uint32 fragmentShader) const;

        int32 getUniformLocation(const char* uniformName) const;
    };
}
