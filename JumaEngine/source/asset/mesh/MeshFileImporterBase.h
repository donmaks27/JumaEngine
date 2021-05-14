// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshFileImporterData.h"
#include "asset/AssetObject.h"
#include "utils/jsubclass.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class Mesh;
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
        bool copyMeshData(const asset_ptr<Mesh>& outMesh, const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferDataClass) const;

    protected:

        jstring m_LoadedMeshFilePath;
        MeshFileImporterData m_Data;


        virtual void importMeshFileInternal() = 0;
    };
}
