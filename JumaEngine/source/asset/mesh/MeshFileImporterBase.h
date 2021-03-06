// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshFileImporterData.h"
#include "Mesh.h"
#include "render/vertexBuffer/VertexBufferData.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class MeshFileImporterBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(MeshFileImporterBase, EngineContextObject)

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
                        VertexBufferDataBase* data = new T();
                        data->copyFromMeshFileImporterData(meshPartData);
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
