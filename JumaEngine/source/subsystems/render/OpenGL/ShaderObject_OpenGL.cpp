// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "ShaderObject_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <fstream>
#include <GL/glew.h>

jutils::jarray<jutils::jstring> LoadOpenGLShaderFile(const jutils::jstring& fileName)
{
    std::ifstream file(*fileName);
    if (!file.is_open())
    {
        return {};
    }

    jutils::jarray<jutils::jstring> result;
    while (!file.eof())
    {
        jutils::jstring line;
        readStreamLine(file, line);
        line += '\n';
        result.add(line);
    }
    file.close();
    return result;
}

namespace JumaEngine
{
    ShaderObject_OpenGL::~ShaderObject_OpenGL()
    {
        clearOpenGL();
    }

    bool ShaderObject_OpenGL::initInternal()
    {
        const jarray<uint32> shaders = {
            compileShader(m_Parent->getVertexShaderName() + JSTR(".vert"), ShaderStage::Vertex),
            compileShader(m_Parent->getFragmentShaderName() + JSTR(".frag"), ShaderStage::Fragment)
        };
        const uint32 programIndex = compileShaderProgram(shaders);
        for (const auto& shader : shaders)
        {
            if (shader != 0)
            {
                glDeleteShader(shader);
            }
        }
        if (programIndex == 0)
        {
            clearOpenGL();
            return false;
        }

        m_ShaderProgramIndex = programIndex;
        return true;
    }
    uint32 ShaderObject_OpenGL::compileShader(const jstring& fileName, const ShaderStage stage, const bool optional) const
    {
        GLenum stageOpenGL;
        switch (stage)
        {
        case ShaderStage::Vertex:   stageOpenGL = GL_VERTEX_SHADER; break;
        case ShaderStage::Geometry: stageOpenGL = GL_GEOMETRY_SHADER; break;
        case ShaderStage::Fragment: stageOpenGL = GL_FRAGMENT_SHADER; break;
        default: 
            JUMA_LOG(error, JSTR("Shader stage not implemented"));
            return 0;
        }

        jarray<jstring> shaderText = LoadOpenGLShaderFile(fileName);
        if (shaderText.isEmpty())
        {
            if (!optional)
            {
                JUMA_LOG(error, JSTR("Empty shader file ") + fileName);
            }
            return 0;
        }

        const uint32 shaderIndex = glCreateShader(stageOpenGL);

        const int32 linesCount = static_cast<int32>(shaderText.getSize());
        jarray<const GLchar*> shader(linesCount);
        jarray<GLint> shaderLineLength(linesCount);
        for (int32 lineIndex = 0; lineIndex < linesCount; lineIndex++)
        {
            const jstring& line = shaderText[lineIndex];
            shader[lineIndex] = *line;
            shaderLineLength[lineIndex] = line.getSize();
        }
        glShaderSource(shaderIndex, linesCount, shader.getData(), shaderLineLength.getData());
        shader.clear();
        shaderLineLength.clear();

        glCompileShader(shaderIndex);

        GLint compileStatus;
        glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE)
        {
#ifndef JUTILS_LOG_DISABLED
            GLint logLength;
            glGetShaderiv(shaderIndex, GL_INFO_LOG_LENGTH, &logLength);

            jstring message(logLength, ' ');
            glGetShaderInfoLog(shaderIndex, logLength, &logLength, *message);
            JUMA_LOG(error, JSTR("Failed to compile shader ") + fileName + JSTR(": ") + message);
#endif
            glDeleteShader(shaderIndex);
            return 0;
        }
        return shaderIndex;
    }
    uint32 ShaderObject_OpenGL::compileShaderProgram(const jarray<uint32>& shaders) const
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
#ifndef JUTILS_LOG_DISABLED
            int logLength;
            glGetProgramiv(shaderProgramIndex, GL_INFO_LOG_LENGTH, &logLength);

            jstring message(logLength, ' ');
            glGetProgramInfoLog(shaderProgramIndex, logLength, &logLength, *message);
            JUMA_LOG(error, JSTR("Failed to compile shader program: ") + message);
#endif
            glDeleteProgram(shaderProgramIndex);
            return 0;
        }
        return shaderProgramIndex;
    }

    void ShaderObject_OpenGL::clearOpenGL()
    {
        if (m_ShaderProgramIndex != 0)
        {
            glDeleteProgram(m_ShaderProgramIndex);
            m_ShaderProgramIndex = 0;
        }
    }

    bool ShaderObject_OpenGL::activate() const
    {
        if (m_ShaderProgramIndex != 0)
        {
            glUseProgram(m_ShaderProgramIndex);
            return true;
        }
        return false;
    }
    void ShaderObject_OpenGL::deactivate()
    {
        glUseProgram(0);
    }
}

#endif
