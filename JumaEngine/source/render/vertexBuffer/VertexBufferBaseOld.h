// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferDescription.h"
#include "engine/EngineContextObject.h"
#include "render/RenderedObject.h"

namespace JumaEngine
{
    class RenderManagerBase;
    class VertexBufferDataBase;

    class VertexBufferBaseOld : public EngineContextObjectOld, public IRenderedObject
    {
        JUMAENGINE_CLASS_OLD(VertexBufferBaseOld, EngineContextObjectOld)

        friend RenderManagerBase;

    public:
        VertexBufferBaseOld() = default;
        virtual ~VertexBufferBaseOld() override = default;

        bool init(VertexBufferDataBase* vertexBufferData);
        bool isInit() const { return m_Initialized; }

        const VertexBufferDescription& getVertexBufferDescription() const { return m_VertexBufferDescription; }

    protected:

        virtual bool initInternal(VertexBufferDataBase* vertexBufferData) = 0;

        virtual void clearWindowData(window_id windowID) {}
        virtual bool hasAnyWindowData() const { return false; }

    private:

        bool m_Initialized = false;
        VertexBufferDescription m_VertexBufferDescription = VertexBufferDescription();
    };
}
