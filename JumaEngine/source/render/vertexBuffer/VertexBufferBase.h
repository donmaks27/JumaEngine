// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferDescription.h"
#include "engine/EngineContextObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class RenderManagerBase;
    class VertexBufferDataBase;

    class VertexBufferBase : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(VertexBufferBase, EngineContextObject)

        friend RenderManagerBase;

    public:
        VertexBufferBase() = default;
        virtual ~VertexBufferBase() override = default;

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
