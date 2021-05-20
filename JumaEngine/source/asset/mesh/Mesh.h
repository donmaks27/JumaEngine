// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/jarray.h"
#include "asset/AssetObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class VertexBufferDataBase;
    class VertexBufferBase;
    class MaterialBase;
    class MeshFileImporterBase;

    class Mesh : public AssetObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(Mesh, AssetObject)

    public:
        Mesh() = default;
        virtual ~Mesh() override;
        
        bool init(const jarray<VertexBufferDataBase*>& meshPartsData);
        bool isInit() const { return !m_VertexBuffers.empty(); }

        asset_ptr<MaterialBase> getMaterial(const uint32 slotIndex) const { return m_Materials.size() > slotIndex ? m_Materials[slotIndex] : asset_ptr<MaterialBase>(); }
        void setMaterial(uint32 slotIndex, const asset_ptr<MaterialBase>& material);

        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    private:

        jarray<VertexBufferBase*> m_VertexBuffers;
        jarray<asset_ptr<MaterialBase>> m_Materials;

        
        void terminateMesh();
    };
}
