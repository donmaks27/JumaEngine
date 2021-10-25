// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "asset/mesh/VertexBufferData.h"
#include <GL/glew.h>

namespace JumaEngine
{
    VertexBuffer_OpenGL::~VertexBuffer_OpenGL()
    {
        if (isValid())
        {
            clearOpenGLBuffers();
        }
    }

    bool VertexBuffer_OpenGL::initInternal(const VertexBufferDataBase* data)
    {
        glGenBuffers(1, &m_VerticesVBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            m_Description.vertexSize * m_Description.verticesCount, 
            data->getVertices(), 
            GL_STATIC_DRAW
        );

        if (m_Description.indicesCount > 0)
        {
            glGenBuffers(1, &m_IndicesVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                sizeof(uint32) * m_Description.indicesCount, 
                data->getVertexIndices(), 
                GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        glGenVertexArrays(1, &m_VerticesVAO);
        glBindVertexArray(m_VerticesVAO);

        const VertexBufferDescription& bufferDescription = getVertexBufferDescription();
        for (uint32 vertexComponentIndex = 0; vertexComponentIndex < bufferDescription.vertexComponents.getSize(); vertexComponentIndex++)
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
        return true;
    }

    void VertexBuffer_OpenGL::clearInternal()
    {
        clearOpenGLBuffers();
    }
    void VertexBuffer_OpenGL::clearOpenGLBuffers()
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

    void VertexBuffer_OpenGL::render(const RenderOptions& options)
    {
        if (!isValid())
        {
            return;
        }

        glBindVertexArray(m_VerticesVAO);
        if (options.invertFacesOrientation)
        {
            glCullFace(GL_FRONT);
        }
        if (m_IndicesVBO == 0)
        {
            glDrawArrays(GL_TRIANGLES, 0, m_Description.verticesCount);
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glDrawElements(GL_TRIANGLES, m_Description.indicesCount, GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
    }
}

#endif
