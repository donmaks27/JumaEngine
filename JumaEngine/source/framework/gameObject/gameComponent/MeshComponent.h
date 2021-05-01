// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "SceneComponent.h"

namespace JumaEngine
{
    class Mesh;

    class MeshComponent : public SceneComponent
    {
    public:
        MeshComponent() = default;
        virtual ~MeshComponent() override = default;

        void setMesh(Mesh* mesh);

        virtual void render() override;

    private:

        typedef SceneComponent Super;

        Mesh* m_Mesh = nullptr;
    };
}
