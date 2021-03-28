// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Mesh.h"
#include "utils/system_functions.h"
#include "framework/material/MaterialBase.h"
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
                m_Materials.add(nullptr);
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

    void Mesh::setMaterial(const uint32 slotIndex, MaterialBase* material)
    {
        if (m_Materials.size() > slotIndex)
        {
            m_Materials[slotIndex] = material;
        }
    }

    void Mesh::draw()
    {
        for (uint32 index = 0; index < m_VertexBuffers.size(); index++)
        {
            VertexBufferBase*& vertexBuffer = m_VertexBuffers[index];
            MaterialBase*& material = m_Materials[index];
            if ((vertexBuffer != nullptr) && (material != nullptr) && vertexBuffer->isInit())
            {
                material->activate();
                if (material->isActive())
                {
                    vertexBuffer->draw();
                }
            }
        }
    }
}
