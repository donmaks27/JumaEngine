// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include <JumaRE/vertex/VertexBuffer.h>

namespace JumaEngine
{
    class Material;
    class MeshesSubsystem;

    struct VertexBuffer
    {
        JumaRE::VertexBuffer* vertexBuffer = nullptr;
        Material* material = nullptr;
    };

    class Mesh : public EngineContextObject
    {
        JUMAENGINE_CLASS(Mesh, EngineContextObject)

        friend MeshesSubsystem;

    public:
        Mesh() = default;
        virtual ~Mesh() override = default;

        JumaRE::VertexBuffer* getVertexBuffer(const uint32 meshSlot) const { return m_MeshSlots.isValidIndex(meshSlot) ? m_MeshSlots[meshSlot].vertexBuffer : nullptr; }

    private:

        jarray<VertexBuffer> m_MeshSlots;


        bool init(const jarray<JumaRE::VertexBufferData>& vertexBuffers, const jarray<Material*>& materials);

        void clearMesh();
    };
}
