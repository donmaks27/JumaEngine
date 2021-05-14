// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshFileImporterBase.h"
#include "Mesh.h"
#include "render/vertexBuffer/VertexBufferData.h"

namespace JumaEngine
{
    void MeshFileImporterBase::importFile(const jstring& filePath, const bool forceLoad)
    {
        if (forceLoad || (filePath != m_LoadedMeshFilePath))
        {
            clear();
            m_LoadedMeshFilePath = filePath;
            if (!m_LoadedMeshFilePath.empty())
            {
                importMeshFileInternal();
            }
        }
    }

    void MeshFileImporterBase::clear()
    {
        m_LoadedMeshFilePath.clear();
        m_Data = MeshFileImporterData();
    }

    bool MeshFileImporterBase::copyMeshData(const asset_ptr<Mesh>& outMesh, const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferDataClass) const
    {
        if ((outMesh == nullptr) || outMesh->isMeshInit() || !bufferDataClass.isValid())
        {
            return false;
        }

        for (const auto& meshData : m_Data.meshesData)
        {
            if (meshData.name == meshName)
            {
                jarray<VertexBufferDataBase*> vertexBuffersData;
                for (const auto& meshPartData : meshData.meshPartsData)
                {
                    VertexBufferDataBase* data = bufferDataClass.createObject();
                    data->copyFromMeshFileImporterData(meshPartData);
                    vertexBuffersData.add(data);
                }
                return outMesh->initMesh(vertexBuffersData);
            }
        }
        return false;
    }
}
