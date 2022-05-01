// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "RenderOptions_OpenGL.h"
#include "RenderSubsystem_OpenGL.h"
#include "subsystems/asyncTasks/ActionTask.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "subsystems/window/OpenGL/WindowSubsystem_OpenGL.h"

namespace JumaEngine
{
    VertexBuffer_RenderAPIObject_OpenGL::~VertexBuffer_RenderAPIObject_OpenGL()
    {
        clearOpenGL();
    }

    bool VertexBuffer_RenderAPIObject_OpenGL::initInternal()
    {
        if (!createVBOs())
        {
            JUMA_LOG(error, JSTR("Failed to create VBO buffers"));
            return false;
        }
        getWindowSubsystemObject()->getParent()->onWindowDestroying.bind(this, &VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying);
        return true;
    }
    bool VertexBuffer_RenderAPIObject_OpenGL::createVBOs()
    {
        const VertexBufferDataBase* data = getVertexData();
        const VertexDescription* vertexDescription = getRenderSubsystemObject()->getParent()->findVertexDescription(data->getVertexName());
        const uint32 indexCount = m_Parent->getIndexCount();

        uint32 verticesVBO = 0, indicesVBO = 0;
        glGenBuffers(1, &verticesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            vertexDescription->size * m_Parent->getVertexCount(), 
            data->getVertices(), 
            GL_STATIC_DRAW
        );

        if (indexCount > 0)
        {
            glGenBuffers(1, &indicesVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                sizeof(uint32) * indexCount, 
                data->getIndices(), 
                GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        m_VerticesVBO = verticesVBO;
        m_IndicesVBO = indicesVBO;
        return true;
    }
    void VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying(const window_id_type windowID)
    {
        m_VerticesVAOs.remove(windowID);
    }
    
    void VertexBuffer_RenderAPIObject_OpenGL::clearOpenGL()
    {
        getWindowSubsystemObject()->getParent()->onWindowDestroying.unbind(this, &VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying);

        const window_id_type prevActiveWindow = getWindowSubsystemObject()->getActiveWindow();
        for (const auto& VAO : m_VerticesVAOs)
        {
            getWindowSubsystemObject()->setWindowActive(VAO.key);
            glDeleteVertexArrays(1, &VAO.value);
        }
        getWindowSubsystemObject()->setWindowActive(prevActiveWindow);
        m_VerticesVAOs.clear();

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

    bool VertexBuffer_RenderAPIObject_OpenGL::render(const RenderOptions* renderOptions)
    {
        const window_id_type windowID = reinterpret_cast<const RenderOptions_OpenGL*>(renderOptions)->windowID;
        const uint32 VAO = getVerticesVAO(windowID);
        if (VAO == 0)
        {
            return false;
        }

        glBindVertexArray(VAO);
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
    uint32 VertexBuffer_RenderAPIObject_OpenGL::getVerticesVAO(const window_id_type windowID)
    {
        const uint32* VAOPtr = m_VerticesVAOs.find(windowID);
        if (VAOPtr != nullptr)
        {
            return *VAOPtr;
        }

        const uint32 VAO = createVerticesVAO();
        return VAO != 0 ? (m_VerticesVAOs[windowID] = VAO) : 0;
    }
    uint32 VertexBuffer_RenderAPIObject_OpenGL::createVerticesVAO() const
    {
        if (m_VerticesVBO == 0)
        {
            return 0;
        }

        const VertexBufferDataBase* data = getVertexData();
        const VertexDescription* vertexDescription = getRenderSubsystemObject()->getParent()->findVertexDescription(data->getVertexName());
        const GLsizei vertexSize = static_cast<GLsizei>(vertexDescription->size);

        uint32 VAO = 0;
        glGenVertexArrays(1, &VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBindVertexArray(VAO);
        for (int32 index = 0; index < vertexDescription->components.getSize(); index++)
        {
            const VertexComponentDescription& componentDescriprion = vertexDescription->components[index];

            GLenum componentType;
            GLint componentSize;
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
                componentDescriprion.location, componentSize, 
                componentType, GL_FALSE, vertexSize, (const void*)static_cast<std::uintptr_t>(componentDescriprion.offset)
            );
            glEnableVertexAttribArray(componentDescriprion.location);
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return VAO;
    }
}

#endif
