// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"
#include "glm/vec3.hpp"

namespace JumaEngine
{
    struct VertexPosition
    {
        glm::vec3 position;
    };

    class VertexBufferDataPosition final : public VertexBufferData<VertexPosition>
    {
    public:
        
        virtual void copyFromVertexBufferImporterData(const VertexBufferImporterMeshPartData& data) override;

        virtual uint32 getVertexComponentsCount() const override { return 1; }
        virtual int32 getVertexComponentID(uint32 vertexComponentIndex) const override;
        virtual VertexComponentType getVertexComponentType(uint32 vertexComponentIndex) const override;
        virtual uint32 getVertexComponentSize(uint32 vertexComponentIndex) const override;
        virtual size_t getVertexComponentOffset(uint32 vertexComponentIndex) const override;
    };
}
