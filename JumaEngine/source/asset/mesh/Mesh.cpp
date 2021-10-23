// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Mesh.h"
#include "asset/material/MaterialBase.h"
#include "render/RenderManagerBase.h"
#include "render/vertexBuffer/VertexBufferBase.h"

namespace JumaEngine
{
    Mesh::~Mesh()
    {
        terminateMesh();
    }

    bool Mesh::init(const jarray<VertexBufferDataBase*>& meshPartsData)
    {
        if (!isInit())
        {
            RenderManagerBase* renderManager = getRenderManager();
		    if (renderManager != nullptr)
		    {
		        for (const auto& vertexBufferData : meshPartsData)
		        {
                    VertexBufferBase* vertexBuffer = renderManager->createVertextBuffer();
                    if (vertexBuffer != nullptr)
                    {
                        vertexBuffer->init(vertexBufferData);
                    }
		            m_VertexBuffers.add(vertexBuffer);
		            m_Materials.add({});
		        }
		        return true;
		    }
        }
        return false;
    }
    void Mesh::terminateMesh()
    {
        RenderManagerBase* renderManager = getRenderManager();
        if (renderManager != nullptr)
        {
            for (auto& vertexBuffer : m_VertexBuffers)
            {
                renderManager->deleteVertexBuffer(vertexBuffer);
            }
        }
        m_VertexBuffers.clear();
        m_Materials.clear();
    }

    void Mesh::setMaterial(const uint32 slotIndex, const asset_ptr<MaterialBase>& material)
    {
        if (m_Materials.getSize() > slotIndex)
        {
            m_Materials[slotIndex] = material;
        }
    }

    void Mesh::render(const window_id windowID, const RenderParams& renderParams)
    {
        for (uint32 index = 0; index < m_VertexBuffers.getSize(); index++)
        {
            VertexBufferBase*& vertexBuffer = m_VertexBuffers[index];
            MaterialBase* material = m_Materials[index].get();
            if ((vertexBuffer != nullptr) && (material != nullptr) && vertexBuffer->isInit())
            {
                if (material->activate())
                {
                	material->loadMaterialParams();
                    vertexBuffer->render(windowID, renderParams);
                	material->deactivate();
                }
            }
        }
    }
}
