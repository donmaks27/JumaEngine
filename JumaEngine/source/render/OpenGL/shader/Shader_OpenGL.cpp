// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Shader_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include <fstream>
#include "GL/glew.h"
#include "utils/jlog.h"
#include "render/texture/TextureBase.h"

namespace JumaEngine
{
    Shader_OpenGL::~Shader_OpenGL()
    {
        clearOpenGLShader();
    }

    bool Shader_OpenGL::loadShaderInternal(const jstring& shaderName)
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

        return m_ShaderProgramIndex != 0;
    }
    void Shader_OpenGL::clearShaderInternal()
    {
        clearOpenGLShader();
    }
    void Shader_OpenGL::clearOpenGLShader()
    {
        if (m_ShaderProgramIndex != 0)
        {
            glDeleteProgram(m_ShaderProgramIndex);
            m_ShaderProgramIndex = 0;
        }
    	m_CachedUniformLocations.clear();
    }

    bool Shader_OpenGL::loadShaderText(const jstring& shaderFilePath, jarray<jstring>& shaderText) const
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
            shaderText.add(line);
        }
        file.close();

        return true;
    }
    uint32 Shader_OpenGL::compileShader(const jarray<jstring>& shaderText, const ShaderType shaderType) const
    {
        uint32 shaderIndex = 0;
        if (!shaderText.empty())
        {
            const int32 linesCount = static_cast<int32>(shaderText.size());
            const GLchar** shader = new const GLchar*[linesCount];
            GLint* shaderLineLength = new GLint[linesCount];
            for (int32 lineIndex = 0; lineIndex < linesCount; lineIndex++)
            {
                const jstring& line = shaderText[lineIndex];
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
#if JLOG_ENABLED
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
    uint32 Shader_OpenGL::loadAndCompileShader(const jstring& shaderFilePath, ShaderType shaderType) const
    {
        uint32 shaderIndex = 0;

        jarray<jstring> shaderText;
        if (loadShaderText(shaderFilePath, shaderText))
        {
            shaderIndex = compileShader(shaderText, shaderType);
#if JLOG_ENABLED
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

    uint32 Shader_OpenGL::compileShaderProgram(const uint32 vertexShader, const uint32 geometryShader, const uint32 fragmentShader) const
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
#if JLOG_ENABLED
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

    void Shader_OpenGL::cacheShaderUniformName(const char* uniformName)
    {
        if (isShaderLoaded() && !m_CachedUniformLocations.contains(uniformName))
        {
            const int32 location = glGetUniformLocation(m_ShaderProgramIndex, uniformName);
    		m_CachedUniformLocations.add(uniformName, location);
        }
    }

    void Shader_OpenGL::activateShaderInternal()
    {
        glUseProgram(m_ShaderProgramIndex);
    }
    void Shader_OpenGL::deactivateShaderOpenGL()
    {
        glUseProgram(0);
    }

    int32 Shader_OpenGL::getUniformLocation(const char* uniformName) const
    {
    	if (isShaderLoaded())
    	{
    		const int32* cachedLocation = m_CachedUniformLocations.findByKey(uniformName);
    		if (cachedLocation != nullptr)
    		{
    			return *cachedLocation;
    		}

    		return glGetUniformLocation(m_ShaderProgramIndex, uniformName);
    	}
        return -1;
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const int32 value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform1i(uniformLocation, value);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const float value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform1f(uniformLocation, value);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const glm::vec2& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform2fv(uniformLocation, 1, &value[0]);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const glm::vec3& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform3fv(uniformLocation, 1, &value[0]);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const glm::vec4& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniform4fv(uniformLocation, 1, &value[0]);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const glm::mat4& value)
    {
        const int32 uniformLocation = getUniformLocation(uniformName);
        if (uniformLocation != -1)
        {
            glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
        }
    }
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const jarray<float>& value)
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
    void Shader_OpenGL::setUniformValueInternal(const char* uniformName, const ShaderUniformTexture& value)
    {
        if (value.texture != nullptr)
        {
            const asset_ptr<TextureBase>& texture = *value.texture;

            static uint32 maxTextureCount = 0;
            if (maxTextureCount == 0)
            {
                int32 count = 0;
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &count);
                maxTextureCount = count;
            }

            if (value.index < maxTextureCount)
            {
                const int32 uniformLocation = getUniformLocation(uniformName);
                if (uniformLocation != -1)
                {
                    texture->activate(value.index);
                    glUniform1i(uniformLocation, value.index);
                }
            }
        }
    }
}

#endif
