// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "asset/AssetObject.h"
#include "utils/jarray.h"

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

        bool initMesh(const jarray<VertexBufferDataBase*>& vertexBuffersData);
        bool isMeshInit() const { return !m_VertexBuffers.empty(); }
        void terminateMesh();

        asset_ptr<MaterialBase> getMaterial(const uint32 slotIndex) const { return m_Materials.size() > slotIndex ? m_Materials[slotIndex] : asset_ptr<MaterialBase>(); }
        void setMaterial(uint32 slotIndex, const asset_ptr<MaterialBase>& material);

        virtual void render();

    protected:

        jarray<VertexBufferBase*> m_VertexBuffers;
        jarray<asset_ptr<MaterialBase>> m_Materials;
    };
}
