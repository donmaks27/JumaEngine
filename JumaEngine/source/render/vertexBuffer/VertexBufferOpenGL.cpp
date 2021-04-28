// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBufferOpenGL.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "VertexBufferData.h"
#include "GL/glew.h"

namespace JumaEngine
{
    VertexBufferOpenGL::~VertexBufferOpenGL()
    {
        clearBuffers();
    }

    void VertexBufferOpenGL::init(VertexBufferDataBase* vertexBufferData)
    {
        if (isInit() || (vertexBufferData == nullptr))
        {
            return;
        }

        if (vertexBufferData->getVerticesCount() > 0)
        {
            m_VerticesCount = vertexBufferData->getVerticesCount();
            const uint32 vertexSize = vertexBufferData->getVertexSize();
            const uint32 vertexComponentsCount = vertexBufferData->getVertexComponentsCount();

            glGenBuffers(1, &m_VerticesVBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
            glBufferData(
                GL_ARRAY_BUFFER, 
                vertexSize * m_VerticesCount, 
                vertexBufferData->getVertices(), 
                GL_STATIC_DRAW
            );

            if (vertexBufferData->shouldUseVertexIndices())
            {
                m_IndicesCount = vertexBufferData->getVertexIndicesCount();

                glGenBuffers(1, &m_IndicesVBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
                glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER, 
                    sizeof(uint32) * m_IndicesCount, 
                    vertexBufferData->getVertexIndices(), 
                    GL_STATIC_DRAW
                );
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glGenVertexArrays(1, &m_VerticesVAO);
            glBindVertexArray(m_VerticesVAO);
            for (uint32 vertexComponentIndex = 0; vertexComponentIndex < vertexComponentsCount; vertexComponentIndex++)
            {
                const int32 componentID = vertexBufferData->getVertexComponentID(vertexComponentIndex);
                if (componentID < 0)
                {
                    continue;
                }

                const VertexComponentType componentTypeEnum = vertexBufferData->getVertexComponentType(vertexComponentIndex);
                if (componentTypeEnum == VertexComponentType::None)
                {
                    continue;
                }

                glVertexAttribPointer(
                    componentID, 
                    vertexBufferData->getVertexComponentSize(vertexComponentIndex), 
                    GL_FLOAT, 
                    GL_FALSE, 
                    vertexSize, 
                    reinterpret_cast<const void*>(vertexBufferData->getVertexComponentOffset(vertexComponentIndex))
                );
                glEnableVertexAttribArray(componentID);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        delete vertexBufferData;
    }
    void VertexBufferOpenGL::terminate()
    {
        clearBuffers();
    }
    void VertexBufferOpenGL::clearBuffers()
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

    void VertexBufferOpenGL::render()
    {
        if (isInit())
        {
            const bool shouldUseVertexIndices = m_IndicesVBO != 0;

            glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
            glBindVertexArray(m_VerticesVAO);

            if (!shouldUseVertexIndices)
            {
                glDrawArrays(GL_TRIANGLES, 0, m_VerticesCount);
            }
            else
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
                glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT, nullptr);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}

#endif
