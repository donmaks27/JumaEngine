// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/jarray.h"
#include "asset/AssetObject.h"
#include "render/RenderedObject.h"

namespace JumaEngine
{
    class VertexBufferDataBase;
    class VertexBufferBaseOld;
    class MaterialBaseOld;
    class MeshFileImporterBase;

    class Mesh : public AssetObject, public IRenderedObject
    {
        JUMAENGINE_CLASS_OLD(Mesh, AssetObject)

    public:
        Mesh() = default;
        virtual ~Mesh() override;
        
        bool init(const jarray<VertexBufferDataBase*>& meshPartsData);
        bool isInit() const { return !m_VertexBuffers.isEmpty(); }

        asset_ptr<MaterialBaseOld> getMaterial(const uint32 slotIndex) const { return m_Materials.isValidIndex(slotIndex) ? m_Materials[slotIndex] : asset_ptr<MaterialBaseOld>(); }
        void setMaterial(uint32 slotIndex, const asset_ptr<MaterialBaseOld>& material);

        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    private:

        jarray<VertexBufferBaseOld*> m_VertexBuffers;
        jarray<asset_ptr<MaterialBaseOld>> m_Materials;

        
        void terminateMesh();
    };
}
