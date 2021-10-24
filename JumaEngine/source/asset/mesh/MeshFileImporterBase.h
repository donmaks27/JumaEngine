// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshFileImporterData.h"
#include "Mesh.h"
#include "VertexBufferData.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class MeshFileImporterBase : public EngineContextObjectOld
    {
        JUMAENGINE_CLASS_OLD(MeshFileImporterBase, EngineContextObjectOld)

    public:
        MeshFileImporterBase() = default;
        virtual ~MeshFileImporterBase() override = default;

        void importFile(const jstring& filePath, bool forceLoad = false);
        void clear();

        const MeshFileImporterData& getData() const { return m_Data; }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<VertexBufferDataBase, T>)>
        bool copyMeshData(const asset_ptr<Mesh>& outMesh, const jstring& meshName) const
        {
            if ((outMesh == nullptr) || outMesh->isInit())
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
                        DefaultVertexBuffer defaultBuffer;
                        for (const auto& vertex : meshPartData.verticesData)
                        {
                            DefaultVertex defaultVertex;
                            defaultVertex.position = vertex.position;
                            defaultVertex.normal = vertex.normal;
                            defaultVertex.textureCoords = vertex.textureCoords;
                            defaultBuffer.vertices.add(defaultVertex);
                        }

                        VertexBufferDataBase* data = new T();
                        data->copyFromDefaultVertexBuffer(defaultBuffer);
                        vertexBuffersData.add(data);
                    }
                    return outMesh->init(vertexBuffersData);
                }
            }
            return false;
        }

    protected:

        jstring m_LoadedMeshFilePath;
        MeshFileImporterData m_Data;


        virtual void importMeshFileInternal() = 0;
    };
}
