// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "RenderInterface.h"

#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class VertexBuffer;
    class Material;

    class RenderPrimitive : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderPrimitive, EngineContextObject)

    public:
        RenderPrimitive() = default;
        virtual ~RenderPrimitive() override = default;

        bool init(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material);
        bool isValid() const { return m_Initialized; }
        void clear();

        const jshared_ptr<VertexBuffer>& getVertexBuffer() const { return m_VertexBuffer; }
        const jshared_ptr<Material>& getMaterial() const { return m_Material; }

    protected:

        virtual bool initInternal(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material) = 0;
        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;
        jshared_ptr<VertexBuffer> m_VertexBuffer;
        jshared_ptr<Material> m_Material;
    };
}
