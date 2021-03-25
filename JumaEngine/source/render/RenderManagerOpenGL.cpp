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
#include "framework/mesh/Mesh.h"
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

        VertexBufferImporterBase* importer = SystemFunctions::getVertexBufferImporter(this);
        importer->load("");
        const std::vector<VertexBufferDataBase*> buffers = importer->createVertexBufferForMesh<VertexBufferDataPosition>(JTEXT("Triangle"));

        m_Mesh = getOwnerEngine()->createObject<Mesh>();
        m_Mesh->initMesh(buffers);
        m_Mesh->setMaterial(0, m_Material);

        return true;
    }

    void RenderManagerOpenGL::render()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_Mesh->draw();
    }

    void RenderManagerOpenGL::terminate()
    {
        if (m_Mesh != nullptr)
        {
            delete m_Mesh;
            m_Mesh = nullptr;
        }
        if (m_Material != nullptr)
        {
            delete m_Material;
            m_Material = nullptr;
        }
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
