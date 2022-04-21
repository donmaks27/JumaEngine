// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/VertexBuffer.h"
#include "OpenGLContextObject.h"

#include <mutex>

#include "jutils/jmap.h"
#include "subsystems/asyncTasks/ActionTask.h"

namespace JumaEngine
{
    class VertexBuffer_RenderAPIObject_OpenGL : public VertexBuffer_RenderAPIObject, public OpenGLContextObject
    {
    public:
        VertexBuffer_RenderAPIObject_OpenGL() = default;
        virtual ~VertexBuffer_RenderAPIObject_OpenGL() override;
        
        virtual bool render(const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;

        virtual bool shouldBeFlushed() const override { return true; }
        virtual void flushChanges() override;

    private:

        uint32 m_VerticesVBO = 0;
        uint32 m_IndicesVBO = 0;
        jmap<window_id_type, uint32> m_VerticesVAOs;

        // Protect m_VerticesVAO_CreateTasks field during render
        std::mutex m_VerticesVAO_ChangesMutex;
        jmap<window_id_type, const ActionTaskResult<uint32>*> m_VerticesVAO_CreateTasks;


        bool createVBOs(uint32& outVerticesVBO, uint32& outIndicesVBO) const;
        void onWindowDestroying(window_id_type windowID);

        void clearOpenGL();
        void clearVerticesVAO(window_id_type windowID, uint32 VAO) const;
        void clearVerticesVAO_CreateTask(window_id_type windowID, const ActionTaskResult<uint32>* createTask) const;
        static void clearVerticesVAO_Static(uint32 VAO);
        static void clearVerticesVAO_CreateTask_Static(const ActionTaskResult<uint32>* createTask);

        uint32 getVerticesVAO(window_id_type windowID);
        uint32 createVerticesVAO() const;
    };
}

#endif
