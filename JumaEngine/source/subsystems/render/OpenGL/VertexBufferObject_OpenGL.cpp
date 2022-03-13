// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VertexBufferObject_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "subsystems/render/vertex/VertexBufferData.h"

namespace JumaEngine
{
    VertexBufferObject_OpenGL::~VertexBufferObject_OpenGL()
    {
        clearOpenGL();
    }

    bool VertexBufferObject_OpenGL::initInternal()
    {
        const VertexBufferDataBase* data = getVertexData();
        const uint32 vertexSize = data->getVertexSize();
        const uint32 indexCount = m_Parent->getIndexCount();
        const jarray<VertexComponentDescription>& vertexComponents = data->getVertexComponents();

        glGenBuffers(1, &m_VerticesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            vertexSize * m_Parent->getVertexCount(), 
            data->getVertices(), 
            GL_STATIC_DRAW
        );

        if (indexCount > 0)
        {
            glGenBuffers(1, &m_IndicesVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                sizeof(uint32) * indexCount, 
                data->getIndices(), 
                GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glGenVertexArrays(1, &m_VerticesVAO);
        glBindVertexArray(m_VerticesVAO);
        for (uint32 index = 0, size = vertexComponents.getSize(); index < size; index++)
        {
            const VertexComponentDescription& componentDescriprion = vertexComponents[index];

            GLenum componentType;
            uint32 componentSize;
            switch (componentDescriprion.type)
            {
            case VertexComponentType::Float: 
                componentType = GL_FLOAT;
                componentSize = 1;
                break;
            case VertexComponentType::Vec2: 
                componentType = GL_FLOAT;
                componentSize = 2;
                break;
            case VertexComponentType::Vec3: 
                componentType = GL_FLOAT;
                componentSize = 3;
                break;
            case VertexComponentType::Vec4: 
                componentType = GL_FLOAT;
                componentSize = 4;
                break;
            default: continue;
            }

            glVertexAttribPointer(
                componentDescriprion.ID, 
                componentSize, componentType, GL_FALSE, 
                vertexSize, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(componentDescriprion.offset))
            );
            glEnableVertexAttribArray(componentDescriprion.ID);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }

    void VertexBufferObject_OpenGL::clearOpenGL()
    {
        if (m_VerticesVAO != 0)
        {
            glDeleteVertexArrays(1, &m_VerticesVAO);
            m_VerticesVAO = 0;
        }
        if (m_IndicesVBO != 0)
        {
            glDeleteBuffers(1, &m_IndicesVBO);
            m_IndicesVBO = 0;
        }
        if (m_VerticesVBO != 0)
        {
            glDeleteBuffers(1, &m_VerticesVBO);
            m_VerticesVBO = 0;
        }
    }

    bool VertexBufferObject_OpenGL::render(const RenderOptions* renderOptions)
    {
        if (m_VerticesVAO == 0)
        {
            return false;
        }

        glBindVertexArray(m_VerticesVAO);
        if (m_IndicesVBO == 0)
        {
            glDrawArrays(GL_TRIANGLES, 0, m_Parent->getVertexCount());
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glDrawElements(GL_TRIANGLES, m_Parent->getIndexCount(), GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
        return true;
    }
}

#endif
