// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/meshes/Mesh.h"

#include "JumaEngine/Engine.h"

namespace JumaEngine
{
    bool Mesh::init(const jarray<JumaRE::VertexBufferData>& vertexBuffers, const jarray<Material*>& materials)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();

        m_MeshSlots.reserve(vertexBuffers.getSize());
        for (int32 index = 0; index < vertexBuffers.getSize(); index++)
        {
            JumaRE::VertexBuffer* vertexBuffer = renderEngine->createVertexBuffer(vertexBuffers[index]);
            if (vertexBuffer == nullptr)
            {
                JUTILS_LOG(error, JSTR("Failed to create vertex buffer"));
                clearMesh();
                return false;
            }
            m_MeshSlots.add({ vertexBuffer, materials.isValidIndex(index) ? materials[index] : nullptr });
        }
        return true;
    }

    void Mesh::clearMesh()
    {
        if (!m_MeshSlots.isEmpty())
        {
            JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
            if (renderEngine != nullptr)
            {
                for (const auto& vertexBuffer : m_MeshSlots)
                {
                    renderEngine->destroyVertexBuffer(vertexBuffer.vertexBuffer);
                }
            }
            m_MeshSlots.clear();
        }
    }
}
