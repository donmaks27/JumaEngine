// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerOpenGL.h"
#include "Engine.h"
#include "GL/glew.h"
#include "utils/log.h"
#include "shader/ShaderOpenGL.h"
#include "vertexBuffer/VertexPosition.h"
#include "vertexBuffer/VertexBufferOpenGL.h"
#include "utils/system_functions.h"
#include "transform/Transform.h"
#include "framework/material/Material.h"
#include "vertexBuffer/importer/VertexBufferImporterBase.h"

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

        m_Material = getOwnerEngine()->createObject<Material>();
        m_Material->setShaderName("content/shaders/testShader");

        /*VertexBufferDataPosition* bufferData = new VertexBufferDataPosition();
        bufferData->vertices = { {{0.0f, -10.0f, -10.0f}}, {{0.0f, 0.0f, -10.0f}}, {{0.0f, 10.0f, 10.0f}} };
        m_VertexBuffer = SystemFunctions::createVertexBuffer(this, bufferData);*/
        VertexBufferImporterBase* importer = SystemFunctions::getVertexBufferImporter(this);
        if (importer != nullptr)
        {
            importer->load("");
            std::vector<VertexBufferDataBase*> buffers = importer->createVertexBufferForMesh<VertexBufferDataPosition>(JTEXT("Triangle"));
            for (auto& bufferData : buffers)
            {
                m_VertexBuffer.push_back(SystemFunctions::createVertexBuffer(this, bufferData));
            }
        }

        return true;
    }

    void RenderManagerOpenGL::render()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_Material->activate();
        if (m_Material->isActive())
        {
            for (auto& vertexBuffer : m_VertexBuffer)
            {
                if (vertexBuffer != nullptr)
                {
                    vertexBuffer->draw();
                }
            }
        }
    }

    void RenderManagerOpenGL::terminate()
    {
        if (m_Material != nullptr)
        {
            delete m_Material;
            m_Material = nullptr;
        }
        for (auto& vertexBuffer : m_VertexBuffer)
        {
            delete vertexBuffer;
        }
        m_VertexBuffer.clear();
    }

    ShaderBase* RenderManagerOpenGL::createShader()
    {
        return new ShaderOpenGL();
    }
    VertexBufferBase* RenderManagerOpenGL::createVertextBufferRender()
    {
        return new VertexBufferOpenGL();
    }
}
