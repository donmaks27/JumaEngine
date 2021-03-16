// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderOpenGL.h"
#include <fstream>
#include <GL/glew.h>

#include "utils/log.h"

namespace JumaEngine
{
    ShaderOpenGL::ShaderOpenGL()
    {
    }
    ShaderOpenGL::~ShaderOpenGL()
    {
    }

    bool ShaderOpenGL::loadShader(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile)
    {
        if (m_ShaderProgramIndex != 0)
        {
            glDeleteProgram(m_ShaderProgramIndex);
            m_ShaderProgramIndex = 0;
        }

        const int32 vertexShader = compileShader(loadShaderText(vertexShaderFile), ShaderType::Vertex);
        const int32 geometryShader = compileShader(loadShaderText(geometryShaderFile), ShaderType::Geometry);
        const int32 fragmentShader = compileShader(loadShaderText(fragmentShaderFile), ShaderType::Fragment);
        m_ShaderProgramIndex = compileShaderProgram(vertexShader, geometryShader, fragmentShader);

        if (vertexShader != 0)
        {
            glDeleteShader(vertexShader);
        }
        if (geometryShader != 0)
        {
            glDeleteShader(geometryShader);
        }
        if (fragmentShader != 0)
        {
            glDeleteShader(fragmentShader);
        }

        return m_ShaderProgramIndex != 0;
    }

    std::vector<std::string> ShaderOpenGL::loadShaderText(const char* shaderFilePath) const
    {
        std::vector<std::string> result;

        std::ifstream file(shaderFilePath);
        if (file.is_open())
        {
            while (!file.eof())
            {
                std::string line;
                getline(file, line);
                line += '\n';
                result.push_back(line);
            }
            file.close();
        }

        return result;
    }
    uint32 ShaderOpenGL::compileShader(const std::vector<std::string>& shaderText, const ShaderType shaderType) const
    {
        uint32 shaderIndex = 0;
        if (!shaderText.empty())
        {
            const int32 linesCount = shaderText.size();
            const GLchar** shader = new const GLchar*[linesCount];
            GLint* shaderLineLength = new GLint[linesCount];
            for (int32 lineIndex = 0; lineIndex < linesCount; lineIndex++)
            {
                const std::string& line = shaderText[lineIndex];
                shader[lineIndex] = line.c_str();
                shaderLineLength[lineIndex] = line.size();
            }

            switch (shaderType)
            {
            case ShaderType::Vertex:   shaderIndex = glCreateShader(GL_VERTEX_SHADER); break;
            case ShaderType::Geometry: shaderIndex = glCreateShader(GL_GEOMETRY_SHADER); break;
            case ShaderType::Fragment: shaderIndex = glCreateShader(GL_FRAGMENT_SHADER); break;
            }
            glShaderSource(shaderIndex, linesCount, shader, shaderLineLength);
            glCompileShader(shaderIndex);

            GLint compileStatus;
            glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &compileStatus);
            if (compileStatus == GL_FALSE)
            {
#if LOG_ENABLED
                GLint logLength;
                glGetShaderiv(shaderIndex, GL_INFO_LOG_LENGTH, &logLength);

                GLchar* logMessage = new GLchar[logLength];
                glGetShaderInfoLog(shaderIndex, logLength, &logLength, logMessage);
                JUMA_LOG(error, logMessage);

                delete[] logMessage;
#endif

                glDeleteShader(shaderIndex);
                shaderIndex = 0;
            }

            delete[] shader;
            delete[] shaderLineLength;
        }
        return shaderIndex;
    }
    uint32 ShaderOpenGL::compileShaderProgram(const uint32 vertexShader, const uint32 geometryShader, const uint32 fragmentShader) const
    {
        uint32 shaderProgramIndex = 0;

        shaderProgramIndex = glCreateProgram();
        if (vertexShader != 0)
        {
            glAttachShader(shaderProgramIndex, vertexShader);
        }
        if (geometryShader != 0)
        {
            glAttachShader(shaderProgramIndex, geometryShader);
        }
        if (fragmentShader != 0)
        {
            glAttachShader(shaderProgramIndex, fragmentShader);
        }
        glLinkProgram(shaderProgramIndex);

        GLint linkStatus;
        glGetProgramiv(shaderProgramIndex, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE)
        {
#if LOG_ENABLED
            int logLength;
            glGetProgramiv(shaderProgramIndex, GL_INFO_LOG_LENGTH, &logLength);

            GLchar* logMessage = new GLchar[logLength];
            glGetProgramInfoLog(shaderProgramIndex, logLength, &logLength, logMessage);
            JUMA_LOG(error, logMessage);

            delete[] logMessage;
#endif

            glDeleteProgram(shaderProgramIndex);
            shaderProgramIndex = 0;
        }

        return shaderProgramIndex;
    }
}
