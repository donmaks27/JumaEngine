// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

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

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete vertexBufferData;
        return true;
    }

    void VertexBuffer_OpenGL::clearWindowData(const window_id windowID)
    {
        if (isInit())
        {
#ifndef JUMAENGINE_SINGLE_WINDOW
            m_VerticesVAOsMutex.lock();

            uint32* VAOPtr = m_VerticesVAOs.findByKey(windowID);
            if (VAOPtr != nullptr)
            {
                glDeleteVertexArrays(1, VAOPtr);
                m_VerticesVAOs.remove(windowID);
            }

            m_VerticesVAOsMutex.unlock();
#else
            if (m_VerticesVAO != 0)
            {
                glDeleteVertexArrays(1, &m_VerticesVAO);
                m_VerticesVAO = 0;
            }
#endif
        }
    }
    bool VertexBuffer_OpenGL::hasAnyWindowData() const
    {
        if (isInit())
        {
#ifndef JUMAENGINE_SINGLE_WINDOW
            m_VerticesVAOsMutex.lock_shared();
            const bool result = !m_VerticesVAOs.empty();
            m_VerticesVAOsMutex.unlock_shared();
            return result;
#else
            return m_VerticesVAO != 0;
#endif
        }
        return false;
    }

    void VertexBuffer_OpenGL::clearBuffers()
    {
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

    void VertexBuffer_OpenGL::render(const window_id windowID, const RenderParams& renderParams)
    {
        if (isInit())
        {
            const VertexBufferDescription& description = getVertexBufferDescription();

            bindVAO(windowID);
            if (renderParams.invertFacesOrientation)
            {
                glCullFace(GL_FRONT);
            }

            if (m_IndicesVBO == 0)
            {
                glDrawArrays(GL_TRIANGLES, 0, description.verticesCount);
            }
            else
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
                glDrawElements(GL_TRIANGLES, description.indicesCount, GL_UNSIGNED_INT, nullptr);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            
            if (renderParams.invertFacesOrientation)
            {
                glCullFace(GL_BACK);
            }
            glBindVertexArray(0);
        }
    }
    void VertexBuffer_OpenGL::bindVAO(const window_id windowID)
    {
#ifndef JUMAENGINE_SINGLE_WINDOW
        m_VerticesVAOsMutex.lock_shared();
        uint32* VAOPtr = m_VerticesVAOs.findByKey(windowID);
        uint32 VAO = VAOPtr != nullptr ? *VAOPtr : 0;
        m_VerticesVAOsMutex.unlock_shared();
#else
        uint32 VAO = m_VerticesVAO;
#endif
        if (VAO != 0)
        {
            glBindVertexArray(VAO);
            return;
        }

        glGenVertexArrays(1, &VAO);
#ifndef JUMAENGINE_SINGLE_WINDOW
        m_VerticesVAOsMutex.lock();
        m_VerticesVAOs.add(windowID, VAO);
        m_VerticesVAOsMutex.unlock();
#else
        m_VerticesVAO = VAO;
#endif
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBindVertexArray(VAO);

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
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

#endif
