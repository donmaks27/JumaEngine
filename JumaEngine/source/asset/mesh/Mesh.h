// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/AssetObject.h"
#include "utils/jarray.h"
#include "engine/EngineContextObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class VertexBufferDataBase;
    class VertexBufferBase;
    class MaterialBase;

    class Mesh : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(Mesh, EngineContextObject)

    public:
        Mesh() = default;
        virtual ~Mesh() override;

        bool initMesh(const jarray<VertexBufferDataBase*>& vertexBuffersData);
        bool isMeshInit() const { return !m_VertexBuffers.empty(); }
        void terminateMesh();

        asset_ptr<MaterialBase> getMaterial(const uint32 slotIndex) const { return m_Materials.size() > slotIndex ? m_Materials[slotIndex] : asset_ptr<MaterialBase>(); }
        void setMaterial(uint32 slotIndex, const asset_ptr<MaterialBase>& material);

        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    private:

        jarray<VertexBufferBase*> m_VertexBuffers;
        jarray<asset_ptr<MaterialBase>> m_Materials;
    };
}
