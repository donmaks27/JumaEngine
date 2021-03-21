// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerOpenGL.h"
#include "Engine.h"
#include "GL/glew.h"
#include "utils/log.h"
#include "shader/ShaderOpenGL.h"
#include "vertexBuffer/VertexPosition.h"
#include "vertexBuffer/VertexBufferOpenGL.h"
#include "utils/system_functions.h"
#include "framework/transform/Transform.h"
#include "framework/gameObject/Camera.h"

namespace JumaEngine
{
    bool RenderManagerOpenGL::init()
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
            return false;
        }

        m_Shader = new ShaderOpenGL();
        m_Shader->loadShader("content/shaders/testShader");

        VertexBufferDataPosition* bufferData = new VertexBufferDataPosition();
        bufferData->vertices = { {{0.0f, -10.0f, -10.0f}}, {{0.0f, 0.0f, -10.0f}}, {{0.0f, 10.0f, 10.0f}} };
        m_VertexBuffer = SystemFunctions::createVertexBuffer(this, bufferData);

		m_Camera = getOwnerEngine()->createObject<Camera>();
    	m_Camera->setWorldLocation({ -50.0f, 0.0f, 0.0f });
    	m_Camera->setWorldRotation({ 0.0f, 0.0f, 0.0f });
    	
        return true;
    }

    void RenderManagerOpenGL::render()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_Shader->activateShader();

        /*const glm::mat4 projMatrix = glm::perspective(3.14f / 2, 4.0f / 3.0f, 0.1f, 100.0f);
        const glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
        const glm::mat4 modelMatrix = Transform({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}).toMatrix();
    	ShaderBase::setActiveShaderUniformValue("uProjection", m_Camera->getProjectionMatrix());
        ShaderBase::setActiveShaderUniformValue("uView", m_Camera->getViewMatrix());
        ShaderBase::setActiveShaderUniformValue("uModel", modelMatrix);

        m_VertexBuffer->draw();

        m_Shader->deactivateShader();
    }

    void RenderManagerOpenGL::terminate()
    {
        if (m_Shader != nullptr)
        {
            delete m_Shader;
            m_Shader = nullptr;
        }
        if (m_VertexBuffer != nullptr)
        {
            delete m_VertexBuffer;
            m_VertexBuffer = nullptr;
        }
    	if (m_Camera != nullptr)
    	{
    		delete m_Camera;
    		m_Camera = nullptr;
    	}
    }

    VertexBufferBase* RenderManagerOpenGL::createVertextBufferRender()
    {
        return new VertexBufferOpenGL();
    }
}
