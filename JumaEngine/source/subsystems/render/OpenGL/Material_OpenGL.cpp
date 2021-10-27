// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>
#include "Shader_OpenGL.h"

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

        const jmap<jstring, ShaderUniform>& uniforms = getShader()->getUniforms();
        for (const auto& nameAndUniform : uniforms)
        {
            switch (nameAndUniform.second.type)
            {
            case ShaderUniformType::Mat4: 
                glm::mat4 value;
                if (getUniformValue(nameAndUniform.first, value))
                {
                    glUniformMatrix4fv(nameAndUniform.second.location, 1, GL_FALSE, &value[0][0]);
                }
                break;

            default: ;
            }
        }
        return true;
    }
    void Material_OpenGL::renderDeactivate(const RenderOptions& options)
    {
        Shader_OpenGL::deactivate();
    }
}

#endif
