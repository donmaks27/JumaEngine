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

        std::mutex m_VerticesVAO_ChangesMutex;
        jmap<window_id_type, const ActionTaskResult<uint32>*> m_VerticesVAO_CreateTasks;


        uint64 createVBOs();
        void createVerticesVAOForWindow(window_id_type windowID);
        uint32 createVerticesVAO();

        void clearOpenGL();
        static void clearVAO(uint32 VAO);

        uint32 getVerticesVAO(window_id_type windowID);
    };
}

#endif
