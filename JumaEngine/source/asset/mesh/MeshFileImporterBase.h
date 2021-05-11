// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshFileImporterData.h"
#include "render/vertexBuffer/VertexBufferData.h"
#include "utils/jarray.h"
#include "utils/jsubclass.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class MeshFileImporterBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(MeshFileImporterBase, EngineContextObject)

    public:
        MeshFileImporterBase() = default;
        virtual ~MeshFileImporterBase() override = default;

        virtual void importMeshFile(const char* filePath) = 0;
        void clear() { m_Data = MeshFileImporterData(); }

        const MeshFileImporterData& getData() const { return m_Data; }
        jarray<VertexBufferDataBase*> createVertexBufferDataForMesh(const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferDataClass);

    protected:

        MeshFileImporterData m_Data;
    };
}
