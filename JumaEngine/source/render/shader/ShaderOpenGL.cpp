// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderOpenGL.h"
#include <fstream>
#include <GL/glew.h>
#include "utils/log.h"

namespace JumaEngine
{
    ShaderOpenGL::ShaderOpenGL()
    {
        m_ShouldAlwaysDeactivateOldShader = false;
    }
    ShaderOpenGL::~ShaderOpenGL()
    {
        clearOpenGLShader();
    }

    void ShaderOpenGL::loadShaderInternal(const std::string& shaderName)
    {
        const uint32 vertexShaderIndex = loadAndCompileShader(shaderName + ".vsh", ShaderType::Vertex);
        const uint32 geometryShaderIndex = loadAndCompileShader(shaderName + ".gsh", ShaderType::Geometry);
        const uint32 fragmentShaderIndex = loadAndCompileShader(shaderName + ".fsh", ShaderType::Fragment);
        m_ShaderProgramIndex = compileShaderProgram(vertexShaderIndex, geometryShaderIndex, fragmentShaderIndex);

        if (vertexShaderIndex != 0)
        {
            glDeleteShader(vertexShaderIndex);
        }
        if (geometryShaderIndex != 0)
        {
            glDeleteShader(geometryShaderIndex);
        }
        if (fragmentShaderIndex != 0)
        {
            glDeleteShader(fragmentShaderIndex);
        }
    }
    void ShaderOpenGL::clearShaderInternal()
    {
        clearOpenGLShader();
    }
    void ShaderOpenGL::clearOpenGLShader()
    {
        if (m_ShaderProgramIndex != 0)
        {
            glDeleteProgram(m_ShaderProgramIndex);
            m_ShaderProgramIndex = 0;
        }
    }

    bool ShaderOpenGL::loadShaderText(const std::string& shaderFilePath, std::vector<std::string>& shaderText) const
    {
        std::ifstream file(shaderFilePath);
        if (!file.is_open())
        {
            return false;
        }

        while (!file.eof())
        {
            std::string line;
            getline(file, line);
            line += '\n';
            shaderText.push_back(line);
        }
        file.close();

        return true;
    }
    uint32 ShaderOpenGL::compileShader(const std::vector<std::string>& shaderText, const ShaderType shaderType) const
    {
        uint32 shaderIndex = 0;
        if (!shaderText.empty())
        {
            const int32 linesCount = static_cast<int32>(shaderText.size());
            const GLchar** shader = new const GLchar*[linesCount];
            GLint* shaderLineLength = new GLint[linesCount];
            for (int32 lineIndex = 0; lineIndex < linesCount; lineIndex++)
            {
                const std::string& line = shaderText[lineIndex];
                shader[lineIndex] = line.c_str();
                shaderLineLength[lineIndex] = static_cast<int32>(line.size());
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
    uint32 ShaderOpenGL::loadAndCompileShader(const std::string& shaderFilePath, ShaderType shaderType) const
    {
        uint32 shaderIndex = 0;

        std::vector<std::string> shaderText;
        if (loadShaderText(shaderFilePath, shaderText))
        {
            shaderIndex = compileShader(shaderText, shaderType);
#if LOG_ENABLED
            if (shaderIndex == 0)
            {
                jstring message = JTEXT("Failed to compile ");
                switch (shaderType)
                {
                case ShaderType::Vertex:   message += JTEXT("vertex"); break;
                case ShaderType::Geometry: message += JTEXT("geometry"); break;
                case ShaderType::Fragment: message += JTEXT("fragment"); break;
                }
                message += JTEXT(" shader - ") + shaderFilePath;
                JUMA_LOG(error, message);
            }
#endif
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

    void ShaderOpenGL::activateShaderInternal()
    {
        glUseProgram(m_ShaderProgramIndex);
    }
    void ShaderOpenGL::deactivateShaderInternal()
    {
        glUseProgram(0);
    }

    int32 ShaderOpenGL::getUniformLocation(const char* uniformName) const
    {
        return m_ShaderProgramIndex != 0 ? glGetUniformLocation(m_ShaderProgramIndex, uniformName) : -1;
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const int32 value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform1i(uniformLocation, value);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const float value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform1f(uniformLocation, value);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const glm::vec2& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform2fv(uniformLocation, 1, &value[0]);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const glm::vec3& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform3fv(uniformLocation, 1, &value[0]);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const glm::vec4& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform4fv(uniformLocation, 1, &value[0]);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const glm::mat4& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
        }
    }
    void ShaderOpenGL::setUniformValue(const char* uniformName, const std::vector<float>& value)
    {
        if (!value.empty())
        {
            const int32 uniformLocation = getUniformLocation(uniformName);
            if (uniformLocation != -1)
            {
                glUniform1fv(uniformLocation, static_cast<GLsizei>(value.size()), value.data());
            }
        }
    }
}
