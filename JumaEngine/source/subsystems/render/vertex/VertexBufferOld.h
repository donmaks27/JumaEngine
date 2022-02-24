// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "VertexComponents.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class VertexBufferOld : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(VertexBufferOld, EngineContextObject)

    public:
        VertexBufferOld() = default;
        virtual ~VertexBufferOld() override = default;

        bool init(const VertexBufferDataBase* data);
        bool isValid() const { return m_Initialized; }
        void clear();

        uint32 getVertexCount() const { return m_VertexCount; }
        uint32 getIndexCount() const { return m_IndexCount; }
        uint32 getVertexSize() const { return m_VertexSize; }
        const jarray<VertexComponentDescription>& getVertexComponents() const { return m_VertexComponents; }

    protected:

        virtual bool initInternal(const VertexBufferDataBase* data) = 0;
        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;

        uint32 m_VertexCount = 0;
        uint32 m_IndexCount = 0;
        uint32 m_VertexSize = 0;
        jarray<VertexComponentDescription> m_VertexComponents;
    };
}
