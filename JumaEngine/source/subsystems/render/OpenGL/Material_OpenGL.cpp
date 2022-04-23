// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderTarget_OpenGL.h"
#include "Shader_OpenGL.h"
#include "Texture_OpenGL.h"
#include "VertexBuffer_OpenGL.h"
#include "engine/Engine.h"
#include "subsystems/render/RenderSubsystem.h"

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
                        glUniformMatrix4fv(static_cast<GLint>(uniform.value.location), 1, GL_FALSE, &value[0][0]);
                    }
                }
                break;
            case ShaderUniformType::Texture:
                {
                    Texture* value = nullptr;
                    m_Parent->getParamValue<ShaderUniformType::Texture>(uniform.key, value);
                    const Texture_RenderAPIObject_OpenGL* textureObject = value != nullptr ? value->getRenderAPIObject<Texture_RenderAPIObject_OpenGL>() : nullptr;
                    if (textureObject == nullptr)
                    {
                        const Texture* defaultTexture = m_Parent->getOwnerEngine()->getRenderSubsystem()->getDefaultTexture();
                        textureObject = defaultTexture != nullptr ? defaultTexture->getRenderAPIObject<Texture_RenderAPIObject_OpenGL>() : nullptr;
                    }
                    if ((textureObject != nullptr) && textureObject->bind(uniform.value.location))
                    {
                        m_ActivatedImageCount = math::max(m_ActivatedImageCount, uniform.value.location + 1);
                    }
                }
                break;
            case ShaderUniformType::RenderTarget:
                {
                    RenderTarget* value = nullptr;
                    m_Parent->getParamValue<ShaderUniformType::RenderTarget>(uniform.key, value);
                    const RenderTarget_RenderAPIObject_OpenGL* renderTargetObject = value != nullptr ? value->getRenderAPIObject<RenderTarget_RenderAPIObject_OpenGL>() : nullptr;
                    if (renderTargetObject == nullptr)
                    {
                        const Texture* defaultTexture = m_Parent->getOwnerEngine()->getRenderSubsystem()->getDefaultTexture();
                        const Texture_RenderAPIObject_OpenGL* textureObject = defaultTexture != nullptr ? defaultTexture->getRenderAPIObject<Texture_RenderAPIObject_OpenGL>() : nullptr;
                        if ((textureObject != nullptr) && textureObject->bind(uniform.value.location))
                        {
                            m_ActivatedImageCount = math::max(m_ActivatedImageCount, uniform.value.location + 1);
                        }
                    }
                    else if (Texture_RenderAPIObject_OpenGL::bind(renderTargetObject->getColorImageIndex(), uniform.value.location))
                    {
                        m_ActivatedImageCount = math::max(m_ActivatedImageCount, uniform.value.location + 1);
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
