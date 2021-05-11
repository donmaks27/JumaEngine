// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MeshFileImporterBase.h"

namespace JumaEngine
{
    class MeshFileImporter_Assimp : public MeshFileImporterBase
    {
        JUMAENGINE_CLASS(MeshFileImporter_Assimp, MeshFileImporterBase)

    public:
        MeshFileImporter_Assimp() = default;
        virtual ~MeshFileImporter_Assimp() override = default;

        virtual void importMeshFile(const char* filePath) override;
    };
}
