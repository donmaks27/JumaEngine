﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct VertexPosition
    {
        glm::vec3 position;
    };

    class VertexBufferDataPosition final : public VertexBufferData<VertexPosition>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override;

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) override;
    };
}
