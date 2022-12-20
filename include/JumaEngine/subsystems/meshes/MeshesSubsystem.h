// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <jutils/jlist.h>

#include "Mesh.h"

namespace JumaEngine
{
    class MeshesSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(MeshesSubsystem, EngineSubsystem)

    public:
        MeshesSubsystem() = default;
        virtual ~MeshesSubsystem() override = default;

        Mesh* generateCudeMesh(Material* material);
        Mesh* generatePlane2DMesh(Material* material);

        void clear();

    protected:

        virtual bool initSubsystem() override;
        virtual void clearSubsystem() override;

    private:

        jlist<Mesh> m_Meshes;
    };
}
