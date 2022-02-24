﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderPrimitive.h"
#include "jutils/jlog.h"

namespace JumaEngine
{
    bool RenderPrimitive::init(const jshared_ptr<VertexBufferOld>& vertexBuffer, const jshared_ptr<MaterialOld>& material)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("RenderPrimitive already initialized"));
            return false;
        }
        if (!initInternal(vertexBuffer, material))
        {
            return false;
        }

        m_VertexBuffer = vertexBuffer;
        m_Material = material;
        m_Initialized = true;
        return true;
    }

    void RenderPrimitive::clear()
    {
        if (isValid())
        {
            clearInternal();

            m_VertexBuffer.reset();
            m_Material.reset();
            m_Initialized = false;
        }
    }
}
