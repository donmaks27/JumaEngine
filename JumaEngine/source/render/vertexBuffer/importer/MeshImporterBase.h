// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshImporterData.h"
#include "render/vertexBuffer/VertexBufferData.h"
#include "utils/jarray.h"
#include "utils/subclass.h"

namespace JumaEngine
{
    class MeshImporterBase
    {
    public:
        MeshImporterBase() = default;
        virtual ~MeshImporterBase() = default;

        void importFile(const char* filePath);

        jarray<jstring> getMeshesNames() const;

        jarray<VertexBufferDataBase*> createVertexBuffersForMesh(const jstring& meshName, const subclass<VertexBufferDataBase>& bufferClass);

    protected:

        virtual MeshImporterData loadDataFromFile(const char* filePath) const;

    private:

        MeshImporterData m_Data;
    };
}
