// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshComponent.h"
#include "framework/mesh/Mesh.h"

namespace JumaEngine
{
    void MeshComponent::setMesh(Mesh* mesh)
    {
        m_Mesh = mesh;
    }

    void MeshComponent::draw()
    {
        if (m_Mesh != nullptr)
        {
            m_Mesh->draw();
        }

        Super::draw();
    }
}
