// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Mesh.h"
#include "utils/system_functions.h"
#include "asset/material/MaterialBase.h"
#include "render/vertexBuffer/VertexBufferBase.h"

namespace JumaEngine
{
    Mesh::~Mesh()
    {
        terminateMesh();
    }

    bool Mesh::initMesh(const jarray<VertexBufferDataBase*>& vertexBuffersData)
    {
        if (!isMeshInit())
        {
            for (const auto& vertexBufferData : vertexBuffersData)
            {
                m_VertexBuffers.add(SystemFunctions::createVertexBuffer(this, vertexBufferData));
                m_Materials.add({});
            }
            return true;
        }
        return false;
    }
    void Mesh::terminateMesh()
    {
        for (auto& vertexBuffer : m_VertexBuffers)
        {
            delete vertexBuffer;
        }
        m_VertexBuffers.clear();
        m_Materials.clear();
    }

    void Mesh::setMaterial(const uint32 slotIndex, const asset_ptr<MaterialBase>& material)
    {
        if (m_Materials.size() > slotIndex)
        {
            m_Materials[slotIndex] = material;
        }
    }

    void Mesh::render(const window_id windowID)
    {
        for (uint32 index = 0; index < m_VertexBuffers.size(); index++)
        {
            VertexBufferBase*& vertexBuffer = m_VertexBuffers[index];
            MaterialBase* material = m_Materials[index].get();
            if ((vertexBuffer != nullptr) && (material != nullptr) && vertexBuffer->isInit())
            {
                material->activate();
                if (material->isActive())
                {
                	material->loadMaterialParams();
                    vertexBuffer->render(windowID);
                	material->deactivate();
                }
            }
        }
    }
}
