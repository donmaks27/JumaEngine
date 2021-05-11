// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct VertexPositionNormal
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class VertexBufferDataPositionNormal final : public VertexBufferData<VertexPositionNormal>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override;

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) override;
    };
}
