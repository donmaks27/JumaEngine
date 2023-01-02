// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <jutils/jlist.h>
#include <jutils/jmap.h>

#include "Mesh.h"
#include "VertexComponent.h"

namespace JumaEngine
{
    class MeshesSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(MeshesSubsystem, EngineSubsystem)

    public:
        MeshesSubsystem() = default;
        virtual ~MeshesSubsystem() override = default;

        jstringID getVertexComponentID(VertexComponent component) const;

        Mesh* generateCudeMesh(Material* material);

        void clear();

    protected:

        virtual bool initSubsystem() override;
        virtual void clearSubsystem() override;

    private:

        jmap<VertexComponent, jstringID> m_VertexComponentIDs;

        jlist<Mesh> m_Meshes;
    };
}
