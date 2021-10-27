// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>
#include "Shader_OpenGL.h"
#include "Image_OpenGL.h"

namespace JumaEngine
{
    Material_OpenGL::~Material_OpenGL()
    {
        if (isValid())
        {
            onMaterialPreClear();
        }
    }

    bool Material_OpenGL::initInternal(const jshared_ptr<Shader>& shader)
    {
        return true;
    }

    void Material_OpenGL::clearInternal()
    {
    }

    bool Material_OpenGL::renderActivate(const RenderOptions& options)
    {
        if (!isValid())
        {
            return false;
        }

        const Shader_OpenGL* shader = EngineContextObject::cast<Shader_OpenGL>(getShader().get());
        if (shader == nullptr)
        {
            return false;
        }

        shader->activate();

        m_ActivatedImagesCount = 0;
        const jmap<jstring, ShaderUniform>& uniforms = getShader()->getUniforms();
        for (const auto& nameAndUniform : uniforms)
        {
            const jstring& name = nameAndUniform.first;
            const uint32& location = nameAndUniform.second.location;
            switch (nameAndUniform.second.type)
            {
            case ShaderUniformType::Mat4:
                {
                    glm::mat4 value;
                    getUniformValue<ShaderUniformType::Mat4>(name, value);
                    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
                }
                break;
            case ShaderUniformType::Image:
                {
                    jshared_ptr<Image> value;
                    getUniformValue<ShaderUniformType::Image>(name, value);
                    const Image_OpenGL* image = cast<Image_OpenGL>(value.get());
                    if (image != nullptr)
                    {
                        image->activate(m_ActivatedImagesCount);
                        glUniform1i(location, m_ActivatedImagesCount);
                        ++m_ActivatedImagesCount;
                    }
                }
                break;

            default: ;
            }
        }
        return true;
    }
    void Material_OpenGL::renderDeactivate(const RenderOptions& options)
    {
        while (m_ActivatedImagesCount > 0)
        {
            --m_ActivatedImagesCount;
            Image_OpenGL::deactivate(m_ActivatedImagesCount);
        }
        Shader_OpenGL::deactivate();
    }
}

#endif
