// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshImporterBase.h"

namespace JumaEngine
{
    void MeshImporterBase::importFile(const char* filePath)
    {
        m_Data = loadDataFromFile(filePath);
    }

    jarray<jstring> MeshImporterBase::getMeshesNames() const
    {
        jarray<jstring> result;
        for (const auto& meshData : m_Data.meshesData)
        {
            result.add(meshData.name);
        }
        return result;
    }

    jarray<VertexBufferDataBase*> MeshImporterBase::createVertexBuffersForMesh(const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferClass)
    {
        jarray<VertexBufferDataBase*> result;
        for (const auto& meshData : m_Data.meshesData)
        {
            if (meshData.name == meshName)
            {
                for (const auto& meshPartData : meshData.meshPartsData)
                {
                    VertexBufferDataBase* vertexBufferData = bufferClass.createObject();
                    vertexBufferData->copyFromVertexBufferImporterData(meshPartData);
                    result.add(vertexBufferData);
                }
                break;
            }
        }
        return result;
    }

    MeshImporterData MeshImporterBase::loadDataFromFile(const char* filePath) const
    {
        const jarray<MeshImporterVertexData> vertices = {
            {{0.0f, -10.0f, -10.0f}},
            {{0.0f, 0.0f, -10.0f}},
            {{0.0f, 10.0f, 10.0f}}
        };
        return { { { JTEXT("Triangle"), Transform(), { { { vertices } } } } } };
    }
}
