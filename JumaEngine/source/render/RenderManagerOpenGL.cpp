// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerOpenGL.h"
#include <GL/glew.h>
#include "utils/log.h"
#include "shader/ShaderOpenGL.h"

namespace JumaEngine
{
    RenderManagerOpenGL::RenderManagerOpenGL()
    {
    }
    RenderManagerOpenGL::~RenderManagerOpenGL()
    {
    }

    bool RenderManagerOpenGL::init()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }

        m_Shader = new ShaderOpenGL();
        m_Shader->loadShader("content/shaders/ui");

        m_Vertex = new float[6]{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f };

        glGenBuffers(1, &m_VertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 3, m_Vertex, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_VertexVAO);
        glBindVertexArray(m_VertexVAO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    void RenderManagerOpenGL::startFrameRender()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_Shader->activateShader();

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
        glBindVertexArray(m_VertexVAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_Shader->deactivateShader();
    }

    void RenderManagerOpenGL::terminate()
    {
        if (m_Shader != nullptr)
        {
            delete m_Shader;
            m_Shader = nullptr;
        }
        if (m_Vertex != nullptr)
        {
            delete[] m_Vertex;
            m_Vertex = nullptr;
        }
    }
}
