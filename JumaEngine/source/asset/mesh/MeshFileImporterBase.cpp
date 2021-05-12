// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshFileImporterBase.h"
#include "render/vertexBuffer/VertexBufferData.h"

namespace JumaEngine
{
    void MeshFileImporterBase::importMeshFile(const jstring& filePath, const bool forceLoad)
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

    jarray<VertexBufferDataBase*> MeshFileImporterBase::createVertexBufferDataForMesh(const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferDataClass) const
    {
        jarray<VertexBufferDataBase*> result;
        for (const auto& meshData : m_Data.meshesData)
        {
            if (meshData.name == meshName)
            {
                for (const auto& meshPartData : meshData.meshPartsData)
                {
                    VertexBufferDataBase* vertexBufferData = bufferDataClass.createObject();
                    vertexBufferData->copyFromMeshFileImporterData(meshPartData);
                    result.add(vertexBufferData);
                }
                break;
            }
        }
        return result;
    }
}
