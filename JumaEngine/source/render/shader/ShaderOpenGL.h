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

        bool loadShader(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile);

    private:

        int32 m_ShaderProgramIndex = 0;


        std::vector<std::string> loadShaderText(const char* shaderFilePath) const;
        uint32 compileShader(const std::vector<std::string>& shaderText, ShaderType shaderType) const;
        uint32 compileShaderProgram(uint32 vertexShader, uint32 geometryShader, uint32 fragmentShader) const;
    };
}
