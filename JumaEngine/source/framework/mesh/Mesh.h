// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include <vector>

namespace JumaEngine
{
    class VertexBufferDataBase;
    class VertexBufferBase;
    class MaterialBase;

    class Mesh : public EngineContextObject
    {
    public:
        Mesh() = default;
        virtual ~Mesh() override;

        bool initMesh(const std::vector<VertexBufferDataBase*>& vertexBuffersData);
        bool isMeshInit() const { return !m_VertexBuffers.empty(); }
        void terminateMesh();

        MaterialBase* getMaterial(const uint32 slotIndex) const { return m_Materials.size() > slotIndex ? m_Materials[slotIndex] : nullptr; }
        void setMaterial(uint32 slotIndex, MaterialBase* material);

        virtual void draw();

    protected:

        std::vector<VertexBufferBase*> m_VertexBuffers;
        std::vector<MaterialBase*> m_Materials;
    };
}
