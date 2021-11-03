// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Mesh.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    bool Mesh::init(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Mesh already initialized"));
            return false;
        }
        if (!initInternal(vertexBuffer, material))
        {
            return false;
        }

        m_VertexBuffer = vertexBuffer;
        m_Material = material;
        return true;
    }

    void Mesh::clear()
    {
        if (isValid())
        {
            clearInternal();

            m_VertexBuffer.reset();
            m_Material.reset();
        }
    }
}
