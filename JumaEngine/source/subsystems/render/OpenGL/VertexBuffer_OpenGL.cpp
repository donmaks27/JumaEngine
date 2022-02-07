﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "subsystems/render/vertexBuffer/VertexBufferData.h"

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
        const uint32 vertexSize = getVertexSize();
        const uint32 indexCount = getIndexCount();
        const jarray<VertexComponentDescription>& vertexComponents = getVertexComponents();

        glGenBuffers(1, &m_VerticesVBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            vertexSize * getVertexCount(), 
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

        const int32 vertexComponentCount = vertexComponents.getSize();
        for (int32 vertexComponentIndex = 0; vertexComponentIndex < vertexComponentCount; vertexComponentIndex++)
        {
            const VertexComponentDescription& componentDescriprion = vertexComponents[vertexComponentIndex];
            GLenum componentType = 0;
            uint32 componentSize = 0;
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

            default:
                continue;
            }

            glVertexAttribPointer(
                componentDescriprion.ID, 
                componentSize, 
                componentType, 
                GL_FALSE, 
                vertexSize, 
                reinterpret_cast<const void*>(static_cast<std::uintptr_t>(componentDescriprion.offset))
            );
            glEnableVertexAttribArray(componentDescriprion.ID);
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

    void VertexBuffer_OpenGL::render() const
    {
        if (!isValid())
        {
            return;
        }
        
        glBindVertexArray(m_VerticesVAO);
        if (m_IndicesVBO == 0)
        {
            glDrawArrays(GL_TRIANGLES, 0, getVertexCount());
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesVBO);
            glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
    }
}

#endif
