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
        // Main thread
        if (!createVBOs(m_VerticesVBO, m_IndicesVBO))
        {
            JUMA_LOG(error, JSTR("Failed to create VBO buffers"));
            return false;
        }
        getWindowSubsystemObject()->getParent()->onWindowDestroying.bind(this, &VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying);
        return true;
    }
    bool VertexBuffer_RenderAPIObject_OpenGL::createVBOs(uint32& outVerticesVBO, uint32& outIndicesVBO) const
    {
        const VertexBufferDataBase* data = getVertexData();
        const uint32 indexCount = m_Parent->getIndexCount();

        uint32 verticesVBO = 0, indicesVBO = 0;
        glGenBuffers(1, &verticesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            static_cast<GLsizei>(data->getVertexSize()) * m_Parent->getVertexCount(), 
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

        outVerticesVBO = verticesVBO;
        outIndicesVBO = indicesVBO;
        return true;
    }
    void VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying(const window_id_type windowID)
    {
        const ActionTaskResult<uint32>* const* createTask = m_VerticesVAO_CreateTasks.find(windowID);
        if (createTask != nullptr)
        {
            clearVerticesVAO_CreateTask(windowID, *createTask);
            m_VerticesVAO_CreateTasks.remove(windowID);
        }

        const uint32* VAOPtr = m_VerticesVAOs.find(windowID);
        if (VAOPtr != nullptr)
        {
            clearVerticesVAO(windowID, *VAOPtr);
            m_VerticesVAOs.remove(windowID);
        }
    }
    
    void VertexBuffer_RenderAPIObject_OpenGL::clearOpenGL()
    {
        // Main thread
        getWindowSubsystemObject()->getParent()->onWindowDestroying.unbind(this, &VertexBuffer_RenderAPIObject_OpenGL::onWindowDestroying);
        for (const auto& task : m_VerticesVAO_CreateTasks)
        {
            clearVerticesVAO_CreateTask(task.key, task.value);
        }
        m_VerticesVAO_CreateTasks.clear();
        for (const auto& VAO : m_VerticesVAOs)
        {
            clearVerticesVAO(VAO.key, VAO.value);
        }
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
    void VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO(const window_id_type windowID, const uint32 VAO) const
    {
        // Main thread
        if (VAO != 0)
        {
            ActionTask deleteTask(true);
            deleteTask.bindRaw(&VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_Static, VAO);
            getWindowSubsystemObject()->submitTaskForWindow(windowID, std::move(deleteTask));
        }
    }
    void VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_CreateTask(const window_id_type windowID, const ActionTaskResult<uint32>* createTask) const
    {
        // Main thread
        if (createTask == nullptr)
        {
            return;
        }

        ActionTask deleteTask(true);
        if (!createTask->isTaskFinished())
        {
            deleteTask.bindRaw(&VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_CreateTask_Static, createTask);
            getWindowSubsystemObject()->submitTaskForWindow(windowID, std::move(deleteTask));
        }
        else
        {
            deleteTask.bindRaw(&VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_Static, createTask->get());
            getWindowSubsystemObject()->submitTaskForWindow(windowID, std::move(deleteTask));
            createTask->markAsHandled();
        }
    }
    void VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_Static(const uint32 VAO)
    {
        // Window thread
        glDeleteVertexArrays(1, &VAO);
    }
    void VertexBuffer_RenderAPIObject_OpenGL::clearVerticesVAO_CreateTask_Static(const ActionTaskResult<uint32>* createTask)
    {
        // Window thread, createTask should be finished by now
        if ((createTask != nullptr) && createTask->isTaskFinished())
        {
            clearVerticesVAO_Static(createTask->get());
            createTask->markAsHandled();
        }
    }

    bool VertexBuffer_RenderAPIObject_OpenGL::render(const RenderOptions* renderOptions)
    {
        // Window thread, during render
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
        // Window thread, during render
        const uint32* VAOPtr = m_VerticesVAOs.find(windowID);
        if (VAOPtr != nullptr)
        {
            return *VAOPtr;
        }

        std::lock_guard lock(m_VerticesVAO_ChangesMutex);
        if (m_VerticesVAO_CreateTasks.contains(windowID))
        {
            return 0;
        }

        ActionTask createTask;
        const ActionTaskResult<uint32>* createTaskResult = createTask.bindClassMethod(this, &VertexBuffer_RenderAPIObject_OpenGL::createVerticesVAO);
        if (getWindowSubsystemObject()->submitTaskForWindow(windowID, std::move(createTask)))
        {
            m_VerticesVAO_CreateTasks.add(windowID, createTaskResult);
        }
        return 0;
    }
    uint32 VertexBuffer_RenderAPIObject_OpenGL::createVerticesVAO() const
    {
        // Window thread
        if (m_VerticesVBO == 0)
        {
            return 0;
        }

        const VertexBufferDataBase* data = getVertexData();
        const GLsizei vertexSize = static_cast<GLsizei>(data->getVertexSize());
        const jarray<VertexComponentDescription>& vertexComponents = data->getVertexComponents();

        uint32 VAO = 0;
        glGenVertexArrays(1, &VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VerticesVBO);
        glBindVertexArray(VAO);
        for (int32 index = 0, size = vertexComponents.getSize(); index < size; index++)
        {
            const VertexComponentDescription& componentDescriprion = vertexComponents[index];

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
                componentDescriprion.ID, 
                componentSize, componentType, GL_FALSE, 
                vertexSize, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(componentDescriprion.offset))
            );
            glEnableVertexAttribArray(componentDescriprion.ID);
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return VAO;
    }
    void VertexBuffer_RenderAPIObject_OpenGL::flushChanges()
    {
        // Main thread, render finished
        std::lock_guard lock(m_VerticesVAO_ChangesMutex);
        if (!m_VerticesVAO_CreateTasks.isEmpty())
        {
            bool hasUnfinishedTasks = false;
            for (auto& task : m_VerticesVAO_CreateTasks)
            {
                if (task.value == nullptr)
                {
                    continue;
                }

                if (!task.value->isTaskFinished())
                {
                    hasUnfinishedTasks = true;
                    continue;
                }

                m_VerticesVAOs.add(task.key, task.value->get());
                task.value->markAsHandled();
                task.value = nullptr;
            }
            if (!hasUnfinishedTasks)
            {
                m_VerticesVAO_CreateTasks.clear();
            }
        }
    }
}

#endif
