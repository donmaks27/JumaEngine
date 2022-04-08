// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jstringID.h"

namespace JumaEngine
{
    struct RenderOptions;
    class VertexBuffer;
    class VertexBufferDataBase;

    class VertexBuffer_RenderAPIObject : public RenderAPIObject<VertexBuffer>
    {
    public:
        VertexBuffer_RenderAPIObject() = default;
        virtual ~VertexBuffer_RenderAPIObject() override = default;

        virtual bool render(const RenderOptions* renderOptions) = 0;

    protected:

        inline const VertexBufferDataBase* getVertexData() const;
    };

    class VertexBuffer final : public EngineContextObject, public RenderAPIWrapper<VertexBuffer_RenderAPIObject>
    {
        JUMAENGINE_CLASS(VertexBuffer, EngineContextObject)

        friend VertexBuffer_RenderAPIObject;

    public:
        VertexBuffer() = default;
        virtual ~VertexBuffer() override;

        bool init(const VertexBufferDataBase* data);

        const jstringID& getVertexName() const { return m_VertexName; }
        uint32 getVertexCount() const { return m_VertexCount; }
        uint32 getIndexCount() const { return m_IndexCount; }

        bool render(const RenderOptions* renderOptions) const;

    protected:

        virtual VertexBuffer_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        const VertexBufferDataBase* m_VertexData = nullptr;
        jstringID m_VertexName = jstringID_NONE;
        uint32 m_VertexCount = 0;
        uint32 m_IndexCount = 0;


        void clearData();
    };
    
    const VertexBufferDataBase* VertexBuffer_RenderAPIObject::getVertexData() const
    {
        return m_Parent->m_VertexData;
    }
}
