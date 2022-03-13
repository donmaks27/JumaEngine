// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "MaterialObject_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "ShaderObject_OpenGL.h"
#include "TextureObject_OpenGL.h"
#include "VertexBufferObject_OpenGL.h"

namespace JumaEngine
{
    bool MaterialObject_OpenGL::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        VertexBufferObject_OpenGL* vertexBufferObject = vertexBuffer != nullptr ? dynamic_cast<VertexBufferObject_OpenGL*>(vertexBuffer->getRenderObject()): nullptr;
        if (vertexBufferObject == nullptr)
        {
            return false;
        }

        if (!activate())
        {
            return false;
        }
        vertexBufferObject->render(renderOptions);
        deactivate();
        return true;
    }

    bool MaterialObject_OpenGL::activate()
    {
        const Shader* shader = m_Parent->getShader();
        const ShaderObject_OpenGL* shaderObject = shader != nullptr ? dynamic_cast<const ShaderObject_OpenGL*>(shader->getRenderObject()) : nullptr;
        if (shaderObject == nullptr)
        {
            JUMA_LOG(error, JSTR("Empty shader object, this shouldn't happen"));
            return false;
        }
        if (!shaderObject->activate())
        {
            return false;
        }

        m_ActivatedImageCount = 0;
        for (const auto& uniform : shader->getUniforms())
        {
            switch (uniform.value.type)
            {
            case ShaderUniformType::Mat4:
                {
                    math::matrix4 value;
                    if (m_Parent->getParamValue<ShaderUniformType::Mat4>(uniform.key, value))
                    {
                        glUniformMatrix4fv(uniform.value.location, 4, GL_FALSE, &value[0][0]);
                    }
                }
                break;
            case ShaderUniformType::Texture:
                {
                    Texture* value = nullptr;
                    if (m_Parent->getParamValue<ShaderUniformType::Texture>(uniform.key, value) && (value != nullptr))
                    {
                        const TextureObject_OpenGL* textureObject = dynamic_cast<const TextureObject_OpenGL*>(value->getRenderObject());
                        if ((textureObject != nullptr) && textureObject->bind(uniform.value.location))
                        {
                            m_ActivatedImageCount = math::max(m_ActivatedImageCount, uniform.value.location + 1);
                        }
                    }
                }
                break;

            default: ;
            }
        }
        return true;
    }
    void MaterialObject_OpenGL::deactivate()
    {
        while (m_ActivatedImageCount > 0)
        {
            TextureObject_OpenGL::unbind(--m_ActivatedImageCount);
        }
        ShaderObject_OpenGL::deactivate();
    }
}

#endif
