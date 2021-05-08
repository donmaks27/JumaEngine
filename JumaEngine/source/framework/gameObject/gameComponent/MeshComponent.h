// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "SceneComponent.h"

namespace JumaEngine
{
    class Mesh;

    class MeshComponent : public SceneComponent
    {
        JUMAENGINE_CLASS(MeshComponent, SceneComponent)

    public:
        MeshComponent() = default;
        virtual ~MeshComponent() override = default;

        void setMesh(Mesh* mesh);

        virtual void render(window_id windowID) override;

    private:

        Mesh* m_Mesh = nullptr;
    };
}
