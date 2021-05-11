// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshFileImporterBase.h"

namespace JumaEngine
{
    jarray<VertexBufferDataBase*> MeshFileImporterBase::createVertexBufferDataForMesh(const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferDataClass)
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
