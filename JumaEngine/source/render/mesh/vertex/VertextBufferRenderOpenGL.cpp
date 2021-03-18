// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertextBufferRenderOpenGL.h"

#include "VertexBufferData.h"
#include "GL/glew.h"

namespace JumaEngine
{
    VertextBufferRenderOpenGL::~VertextBufferRenderOpenGL()
    {
        clearBuffers();
    }

    bool VertextBufferRenderOpenGL::initInternal()
    {
        const uint32 verticesCount = m_VertexBufferData->getVerticesCount();
        const uint32 vertexSize = m_VertexBufferData->getVertexSize();
        const uint32 vertexComponentsCount = m_VertexBufferData->getVertexComponentsCount();

        glGenBuffers(1, &m_VerticesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            vertexSize * verticesCount, 
            m_VertexBufferData->getVertices(), 
            GL_STATIC_DRAW
        );

        if (m_VertexBufferData->shouldUseVertexIndices())
        {
            glGenBuffers(1, &m_IndicesVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                sizeof(uint32) * m_VertexBufferData->getVertexIndicesCount(), 
                m_VertexBufferData->getVertexIndices(), 
                GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glGenVertexArrays(1, &m_VerticesVAO);
        glBindVertexArray(m_VerticesVAO);
        for (uint32 vertexComponentIndex = 0; vertexComponentIndex < vertexComponentsCount; vertexComponentIndex++)
        {
            const int32 componentID = m_VertexBufferData->getVertexComponentID(vertexComponentIndex);
            if (componentID < 0)
            {
                continue;
            }

            const VertexComponentType componentTypeEnum = m_VertexBufferData->getVertexComponentType(vertexComponentIndex);
            if (componentTypeEnum == VertexComponentType::None)
            {
                continue;
            }

            glVertexAttribPointer(
                componentID, 
                m_VertexBufferData->getVertexComponentSize(vertexComponentIndex), 
                GL_FLOAT, 
                GL_FALSE, 
                vertexSize, 
                reinterpret_cast<const void*>(m_VertexBufferData->getVertexComponentOffset(vertexComponentIndex))
            );
            glEnableVertexAttribArray(componentID);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }
    void VertextBufferRenderOpenGL::terminateInternal()
    {
        clearBuffers();
    }
    void VertextBufferRenderOpenGL::clearBuffers()
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

    void VertextBufferRenderOpenGL::draw()
    {
        if (isInit())
        {
            const bool shouldUseVertexIndices = m_VertexBufferData->shouldUseVertexIndices();

            glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
            glBindVertexArray(m_VerticesVAO);

            if (!shouldUseVertexIndices)
            {
                glDrawArrays(GL_TRIANGLES, 0, m_VertexBufferData->getVerticesCount());
            }
            else
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
                glDrawElements(GL_TRIANGLES, m_VertexBufferData->getVertexIndicesCount(), GL_UNSIGNED_INT, nullptr);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}
