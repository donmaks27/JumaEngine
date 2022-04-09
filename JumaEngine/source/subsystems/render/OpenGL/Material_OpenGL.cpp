// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderTarget_OpenGL.h"
#include "Shader_OpenGL.h"
#include "Texture_OpenGL.h"
#include "VertexBuffer_OpenGL.h"

namespace JumaEngine
{
    bool Material_RenderAPIObject_OpenGL::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        if (vertexBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        if (!activate())
        {
            return false;
        }

        VertexBuffer_RenderAPIObject_OpenGL* vertexBufferObject = vertexBuffer->getRenderAPIObject<VertexBuffer_RenderAPIObject_OpenGL>();
        vertexBufferObject->render(renderOptions);

        deactivate();
        return true;
    }

    bool Material_RenderAPIObject_OpenGL::activate()
    {
        const Shader* shader = m_Parent->getShader();
        const Shader_RenderAPIObject_OpenGL* shaderObject = shader->getRenderAPIObject<Shader_RenderAPIObject_OpenGL>();
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
                        glUniformMatrix4fv(static_cast<GLint>(uniform.value.location), 4, GL_FALSE, &value[0][0]);
                    }
                }
                break;
            case ShaderUniformType::Texture:
                {
                    Texture* value = nullptr;
                    if (m_Parent->getParamValue<ShaderUniformType::Texture>(uniform.key, value) && (value != nullptr))
                    {
                        const Texture_RenderAPIObject_OpenGL* textureObject = value->getRenderAPIObject<Texture_RenderAPIObject_OpenGL>();
                        if ((textureObject != nullptr) && textureObject->bind(uniform.value.location))
                        {
                            m_ActivatedImageCount = math::max(m_ActivatedImageCount, uniform.value.location + 1);
                        }
                    }
                }
                break;
            case ShaderUniformType::RenderTarget:
                {
                    RenderTarget* value = nullptr;
                    if (m_Parent->getParamValue<ShaderUniformType::RenderTarget>(uniform.key, value) && (value != nullptr))
                    {
                        const RenderTarget_RenderAPIObject_OpenGL* renderTargetObject = value->getRenderAPIObject<RenderTarget_RenderAPIObject_OpenGL>();
                        if ((renderTargetObject != nullptr) && Texture_RenderAPIObject_OpenGL::bind(renderTargetObject->getColorImageIndex(), uniform.value.location))
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
    void Material_RenderAPIObject_OpenGL::deactivate()
    {
        while (m_ActivatedImageCount > 0)
        {
            Texture_RenderAPIObject_OpenGL::unbind(--m_ActivatedImageCount);
        }
        Shader_RenderAPIObject_OpenGL::deactivate();
    }
}

#endif
