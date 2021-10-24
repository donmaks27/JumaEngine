// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferDescription.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class VertexBuffer : public EngineContextObject
    {
        JUMAENGINE_CLASS(VertexBuffer, EngineContextObject)

    public:
        VertexBuffer() = default;
        virtual ~VertexBuffer() override = default;

        bool init(const VertexBufferDataBase* data);
        bool isValid() const { return m_Initialized; }

        const VertexBufferDescription& getVertexBufferDescription() const { return m_Description; }

    protected:

        virtual bool initInternal(const VertexBufferDataBase* data) = 0;

    private:

        bool m_Initialized = false;
        VertexBufferDescription m_Description = VertexBufferDescription();
    };
}
