// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_OpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "render/vertexBuffer/VertexBufferData.h"
#include "GL/glew.h"

namespace JumaEngine
{
    VertexBuffer_OpenGL::~VertexBuffer_OpenGL()
    {
        clearBuffers();
    }

    bool VertexBuffer_OpenGL::initInternal(VertexBufferDataBase* vertexBufferData)
    {
        const VertexBufferDescription& bufferDescription = getVertexBufferDescription();

        glGenBuffers(1, &m_VerticesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            bufferDescription.vertexSize * bufferDescription.verticesCount, 
            vertexBufferData->getVertices(), 
            GL_STATIC_DRAW
        );

        if (bufferDescription.indicesCount > 0)
        {
            glGenBuffers(1, &m_IndicesVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                sizeof(uint32) * bufferDescription.indicesCount, 
                vertexBufferData->getVertexIndices(), 
                GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glGenVertexArrays(1, &m_VerticesVAO);
        glBindVertexArray(m_VerticesVAO);
        for (uint32 vertexComponentIndex = 0; vertexComponentIndex < bufferDescription.vertexComponents.size(); vertexComponentIndex++)
        {
            const VertexComponentDescription& componentDescriprion = bufferDescription.vertexComponents[vertexComponentIndex];
            if (!componentDescriprion.isValid())
            {
                continue;
            }

            GLenum componentType = 0;
            switch (componentDescriprion.componentType)
            {
            case VertexComponentType::Float: 
                componentType = GL_FLOAT;
                break;

            case VertexComponentType::None:
            default:
                continue;
            }

            glVertexAttribPointer(
                componentDescriprion.componentID, 
                componentDescriprion.componentSize, 
                componentType, 
                GL_FALSE, 
                bufferDescription.vertexSize, 
                reinterpret_cast<const void*>(componentDescriprion.componentOffset)
            );
            glEnableVertexAttribArray(componentDescriprion.componentID);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete vertexBufferData;
        return true;
    }

    void VertexBuffer_OpenGL::clearBuffers()
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

    void VertexBuffer_OpenGL::render(const window_id windowID)
    {
        if (isInit())
        {
            const VertexBufferDescription& description = getVertexBufferDescription();
            const bool shouldUseVertexIndices = m_IndicesVBO != 0;

            glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
            glBindVertexArray(m_VerticesVAO);

            if (!shouldUseVertexIndices)
            {
                glDrawArrays(GL_TRIANGLES, 0, description.verticesCount);
            }
            else
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
                glDrawElements(GL_TRIANGLES, description.indicesCount, GL_UNSIGNED_INT, nullptr);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}

#endif
