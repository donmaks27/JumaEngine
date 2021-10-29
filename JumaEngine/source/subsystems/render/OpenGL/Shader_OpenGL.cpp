// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Shader_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <fstream>
#include <GL/glew.h>
#include "utils/jarray.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    Shader_OpenGL::~Shader_OpenGL()
    {
        if (isValid())
        {
            onShaderPreClear();
            clearOpenGL();
        }
    }

    bool Shader_OpenGL::initInternal(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms)
    {
        jarray<uint32> shaders(3, 0);
        loadAndCompileShader(shaderName + ".vsh", ShaderStage::Vertex, shaders[0]);
        loadAndCompileShader(shaderName + ".gsh", ShaderStage::Geometry, shaders[1]);
        loadAndCompileShader(shaderName + ".fsh", ShaderStage::Fragment, shaders[2]);
        const bool result = compileShaderProgram(shaders, m_ShaderProgramIndex);
        for (const auto& shader : shaders)
        {
            if (shader != 0)
            {
                glDeleteShader(shader);
            }
        }
        return result;
    }

    bool Shader_OpenGL::loadAndCompileShader(const jstring& shaderFilePath, const ShaderStage shaderType, uint32& outShaderIndex) const
    {
        jarray<jstring> shaderText;
        if (!loadShaderText(shaderFilePath, shaderText))
        {
            return false;
        }
        if (!compileShader(shaderText, shaderType, outShaderIndex))
        {
#if JLOG_ENABLED
            jstring shaderTypeString;
            switch (shaderType)
            {
            case ShaderStage::Vertex:   shaderTypeString = JSTR("vertex"); break;
            case ShaderStage::Geometry: shaderTypeString = JSTR("geometry"); break;
            case ShaderStage::Fragment: shaderTypeString = JSTR("fragment"); break;
            default: shaderTypeString = JSTR("NONE"); break;
            }
            JUMA_LOG(error, JSTR("Failed to compile ") + shaderTypeString + JSTR(" shader ") + shaderFilePath);
#endif
            return false;
        }
        return true;
    }
    bool Shader_OpenGL::loadShaderText(const jstring& shaderFilePath, jarray<jstring>& outShaderText) const
    {
        std::ifstream file(shaderFilePath);
        if (!file.is_open())
        {
            return false;
        }

        while (!file.eof())
        {
            jstring line;
            getline(file, line);
            line += '\n';
            outShaderText.add(line);
        }
        file.close();
        return true;
    }
    bool Shader_OpenGL::compileShader(const jarray<jstring>& shaderText, const ShaderStage shaderType, uint32& outShaderIndex) const
    {
        if (shaderText.isEmpty())
        {
            JUMA_LOG(error, JSTR("Shader is empty"));
            return false;
        }

        uint32 shaderIndex;
        switch (shaderType)
        {
        case ShaderStage::Vertex:   shaderIndex = glCreateShader(GL_VERTEX_SHADER); break;
        case ShaderStage::Geometry: shaderIndex = glCreateShader(GL_GEOMETRY_SHADER); break;
        case ShaderStage::Fragment: shaderIndex = glCreateShader(GL_FRAGMENT_SHADER); break;
        default: 
            JUMA_LOG(error, JSTR("Unknown shader stage"));
            return false;
        }

        const int32 linesCount = static_cast<int32>(shaderText.getSize());
        jarray<const GLchar*> shader(linesCount);
        jarray<GLint> shaderLineLength(linesCount);
        for (int32 lineIndex = 0; lineIndex < linesCount; lineIndex++)
        {
            const jstring& line = shaderText[lineIndex];
            shader[lineIndex] = line.c_str();
            shaderLineLength[lineIndex] = static_cast<int32>(line.size());
        }
        glShaderSource(shaderIndex, linesCount, shader.getData(), shaderLineLength.getData());
        shader.clear();
        shaderLineLength.clear();

        glCompileShader(shaderIndex);

        GLint compileStatus;
        glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE)
        {
#if JLOG_ENABLED
            GLint logLength;
            glGetShaderiv(shaderIndex, GL_INFO_LOG_LENGTH, &logLength);

            GLchar* logMessage = new GLchar[logLength];
            glGetShaderInfoLog(shaderIndex, logLength, &logLength, logMessage);
            JUMA_LOG(error, logMessage);

            delete[] logMessage;
#endif
            glDeleteShader(shaderIndex);
            return false;
        }

        outShaderIndex = shaderIndex;
        return true;
    }
    bool Shader_OpenGL::compileShaderProgram(const jarray<uint32>& shaders, uint32& outShaderProgramIndex) const
    {
        const uint32 shaderProgramIndex = glCreateProgram();
        for (const auto& shader : shaders)
        {
            if (shader != 0)
            {
                glAttachShader(shaderProgramIndex, shader);
            }
        }
        glLinkProgram(shaderProgramIndex);

        GLint linkStatus;
        glGetProgramiv(shaderProgramIndex, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE)
        {
#if JLOG_ENABLED
            int logLength;
            glGetProgramiv(shaderProgramIndex, GL_INFO_LOG_LENGTH, &logLength);

            GLchar* logMessage = new GLchar[logLength];
            glGetProgramInfoLog(shaderProgramIndex, logLength, &logLength, logMessage);
            JUMA_LOG(error, logMessage);

            delete[] logMessage;
#endif
            glDeleteProgram(shaderProgramIndex);
            return false;
        }

        outShaderProgramIndex = shaderProgramIndex;
        return true;
    }

    void Shader_OpenGL::clearInternal()
    {
        clearOpenGL();
    }
    void Shader_OpenGL::clearOpenGL()
    {
        if (m_ShaderProgramIndex != 0)
        {
            glDeleteProgram(m_ShaderProgramIndex);
            m_ShaderProgramIndex = 0;
        }
    }

    void Shader_OpenGL::activate() const
    {
        if (isValid())
        {
            glUseProgram(m_ShaderProgramIndex);
        }
    }
    void Shader_OpenGL::deactivate()
    {
        glUseProgram(0);
    }
}

#endif
