// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshImporterData.h"
#include "render/vertexBuffer/VertexBufferData.h"
#include "utils/jarray.h"
#include "utils/jsubclass.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class MeshImporterBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(MeshImporterBase, EngineContextObject)

    public:
        MeshImporterBase() = default;
        virtual ~MeshImporterBase() override = default;

        void importFile(const char* filePath);

        jarray<jstring> getMeshesNames() const;

        jarray<VertexBufferDataBase*> createVertexBuffersForMesh(const jstring& meshName, const jsubclass<VertexBufferDataBase>& bufferClass);

    protected:

        virtual MeshImporterData loadDataFromFile(const char* filePath) const;

    private:

        MeshImporterData m_Data;
    };
}
