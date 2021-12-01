// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/AssetObject.h"
#include "SceneComponent.h"

namespace JumaEngine
{
    class MeshOld;

    class MeshComponent : public SceneComponent
    {
        JUMAENGINE_CLASS_OLD(MeshComponent, SceneComponent)

    public:
        MeshComponent() = default;
        virtual ~MeshComponent() override = default;

        void setMesh(const asset_ptr<MeshOld>& mesh);

        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    private:

        asset_ptr<MeshOld> m_Mesh = nullptr;
    };
}
